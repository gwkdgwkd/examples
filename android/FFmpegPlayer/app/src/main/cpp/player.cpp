#include "player.h"
#include "log.h"

static Player *player;
static void AudioRenderCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    if (player->GetAudioRender() == nullptr) return;
    if (!player->GetAudioRender()->IsQueueSelf(bq)) return;
    if (player->GetAudioRender()->IsQueueLooping()) {
        AudioFrame *frame = player->GetAudioDecorder()->GetAudioFrame();
        if (frame) {
            player->GetAudioRender()->SendQueueLoop(frame->data_, frame->data_size_);
        }
    }
}

Player::Player() {
    demuxer_ = nullptr;
    audio_decoder_ = nullptr;
    audio_render_ = nullptr;
}

Player::~Player() {}

bool Player::Init(const char* url) {
    TRACE_FUNC();
    int ret = false;

    demuxer_ = new FFmpegDemuxer(url);
    if(!demuxer_->Init()) {
        LOGE("FFmpegDemuxer init failed!");
        return ret;
    }

    audio_decoder_ = new FFmpegAudioDecorder(demuxer_);
    if(!audio_decoder_->Init()) {
        LOGE("FFmpegAudioDecorder init failed!");
        return ret;
    }

    audio_render_ = new SLRender();
    audio_render_->SetQueueCallBack(AudioRenderCallback);

    player = this;

    return true;
}

void Player::Start() {
    TRACE_FUNC();

    demuxer_->Start();

    audio_decoder_->Start();

    audio_render_->Start();
    audio_render_->SendQueueLoop("", 1);    // 开启轮询
}

FFmpegAudioDecorder *Player::GetAudioDecorder() const{
    return audio_decoder_;
};

SLBase *Player::GetAudioRender() const{
    return audio_render_;
}


