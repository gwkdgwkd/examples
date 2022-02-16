#ifndef FFMPEG_PLAYER_SEEK_OBSERVER_INTERFACE_H
#define FFMPEG_PLAYER_SEEK_OBSERVER_INTERFACE_H

class SeekObserverInterface {
public:
    virtual ~SeekObserverInterface() = default;
    virtual void OnSeekTo(float position) = 0;
};

#endif // FFMPEG_PLAYER_SEEK_OBSERVER_INTERFACE_H
