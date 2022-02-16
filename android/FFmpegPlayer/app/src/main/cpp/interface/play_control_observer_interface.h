#ifndef FFMPEG_PLAYER_PLAY_CONTROL_OBSERVER_INTERFACE_H
#define FFMPEG_PLAYER_PLAY_CONTROL_OBSERVER_INTERFACE_H

class PlayControlObserverInterface {
public:
    enum ControlType {
        kPlay,
        kStop,
        kPause,
        kResume
    };
    virtual ~PlayControlObserverInterface() = default;

    virtual void OnControlEvent(ControlType type) = 0;
};

#endif // FFMPEG_PLAYER_PLAY_CONTROL_OBSERVER_INTERFACE_H
