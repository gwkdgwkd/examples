#ifndef FFMPEG_PLAYER_PLAY_CONTROL_OBSERVER_INTERFACE_H
#define FFMPEG_PLAYER_PLAY_CONTROL_OBSERVER_INTERFACE_H

class PlayControlObserverInterface {
public:
    virtual ~PlayControlObserverInterface() = default;

    virtual void OnPlay() = 0;
    virtual void OnStop() = 0;
    virtual void OnPause() = 0;
    virtual void OnResume() = 0;
    virtual void OnSeekTo(float position) = 0;
};

#endif // FFMPEG_PLAYER_PLAY_CONTROL_OBSERVER_INTERFACE_H
