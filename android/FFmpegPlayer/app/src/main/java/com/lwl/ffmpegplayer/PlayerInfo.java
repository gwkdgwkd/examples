package com.lwl.ffmpegplayer;

import java.io.Serializable;

public class PlayerInfo implements Serializable {
    private ViewType viewType;
    private AudioRenderType audioRenderType;
    private ViedoRenderType viedoRenderType;
    private EffectType effectType;
    private ScaleType scaleType;

    public PlayerInfo(ViewType viewType, AudioRenderType audioRenderType, ViedoRenderType viedoRenderType, EffectType effectType, ScaleType scaleType) {
        this.viewType = viewType;
        this.audioRenderType = audioRenderType;
        this.viedoRenderType = viedoRenderType;
        this.effectType = effectType;
        this.scaleType = scaleType;
    }

    public ViewType getViewType() {
        return viewType;
    }

    public void setViewType(ViewType viewType) {
        this.viewType = viewType;
    }

    public AudioRenderType getAudioRenderType() {
        return audioRenderType;
    }

    public void setAudioRenderType(AudioRenderType audioRenderType) {
        this.audioRenderType = audioRenderType;
    }

    public ViedoRenderType getViedoRenderType() {
        return viedoRenderType;
    }

    public void setViedoRenderType(ViedoRenderType viedoRenderType) {
        this.viedoRenderType = viedoRenderType;
    }

    public EffectType getEffectType() {
        return effectType;
    }

    public void setEffectType(EffectType effectType) {
        this.effectType = effectType;
    }

    public ScaleType getScaleType() {
        return scaleType;
    }

    public void setScaleType(ScaleType scaleType) {
        this.scaleType = scaleType;
    }
}
