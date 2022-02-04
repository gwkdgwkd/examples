#include "packet_queue.h"
#include "log.h"

int packet_queue_put_private(PacketQueue *q, AVPacket *pkt) {
    MyAVPacketList pkt1;

    std::unique_lock <std::mutex> lk(q->mutex);
    if (q->abort_request)
        return -1;
    if (av_fifo_space(q->pkt_list) < sizeof(pkt1)) {
        if (av_fifo_grow(q->pkt_list, sizeof(pkt1)) < 0)
            return -1;
    }
    pkt1.pkt = pkt;
    pkt1.serial = q->serial;
    av_fifo_generic_write(q->pkt_list, &pkt1, sizeof(pkt1), NULL);
    q->nb_packets++;
    q->size += pkt1.pkt->size + sizeof(pkt1);
    q->duration += pkt1.pkt->duration;
    q->cond.notify_all();
    return 0;
}

int packet_queue_put(PacketQueue *q, AVPacket *pkt) {
    AVPacket *pkt1;
    int ret;

    pkt1 = av_packet_alloc();
    if (!pkt1) {
        av_packet_unref(pkt);
        return -1;
    }
    av_packet_move_ref(pkt1, pkt);

    ret = packet_queue_put_private(q, pkt1);

    if (ret < 0)
        av_packet_free(&pkt1);

    return ret;
}

int packet_queue_put_nullpacket(PacketQueue *q, AVPacket *pkt, int stream_index) {
    pkt->stream_index = stream_index;
    return packet_queue_put(q, pkt);
}

/* packet queue handling */
int packet_queue_init(PacketQueue *q) {
    memset(q, 0, sizeof(PacketQueue));
    q->pkt_list = av_fifo_alloc(sizeof(MyAVPacketList));
    if (!q->pkt_list)
        return AVERROR(ENOMEM);

    q->abort_request = 0;
    return 0;
}

void packet_queue_flush(PacketQueue *q) {
    std::unique_lock <std::mutex> lk(q->mutex);
    TRACE_FUNC();
    MyAVPacketList pkt1;


    while (av_fifo_size(q->pkt_list) >= sizeof(pkt1)) {
        av_fifo_generic_read(q->pkt_list, &pkt1, sizeof(pkt1), NULL);
        av_packet_free(&pkt1.pkt);
    }
    q->nb_packets = 0;
    q->size = 0;
    q->duration = 0;
    q->serial++;
}

void packet_queue_destroy(PacketQueue *q) {
    packet_queue_flush(q);
    av_fifo_freep(&q->pkt_list);
}

void packet_queue_abort(PacketQueue *q) {
    std::unique_lock <std::mutex> lk(q->mutex);
    q->abort_request = 1;
    q->cond.notify_all();
}

void packet_queue_start(PacketQueue *q) {
    std::unique_lock <std::mutex> lk(q->mutex);
    q->abort_request = 0;
    q->serial++;
}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block, int *serial) {
    std::unique_lock <std::mutex> lk(q->mutex);
    MyAVPacketList pkt1;
    int ret;

    for (;;) {
        if (q->abort_request) {
            ret = -1;
            break;
        }

        if (av_fifo_size(q->pkt_list) >= sizeof(pkt1)) {
            av_fifo_generic_read(q->pkt_list, &pkt1, sizeof(pkt1), NULL);
            q->nb_packets--;
            q->size -= pkt1.pkt->size + sizeof(pkt1);
            q->duration -= pkt1.pkt->duration;
            av_packet_move_ref(pkt, pkt1.pkt);
            if (serial)
                *serial = pkt1.serial;
            av_packet_free(&pkt1.pkt);
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            q->cond.wait(lk);
            lk.unlock();
        }
    }

    return ret;
}

