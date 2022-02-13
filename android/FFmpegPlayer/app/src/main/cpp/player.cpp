#include "player.h"
#include "log.h"

static Player *player;
static JavaVM *gl_jvm = NULL;
static jobject gl_object = NULL;

#define JAVA_PLAYER_EVENT_CALLBACK_API_NAME "playerEventCallback"

static void AudioRenderCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    if (player->GetAudioRender() == nullptr) return;
    if (!player->GetAudioRender()->IsQueueSelf(bq)) return;
    if (player->GetAudioRender()->IsQueueLooping()) {
        AudioFrame *frame = player->GetAudioDecoder()->GetAudioFrame();
        if (frame) {
            player->GetAudioRender()->SendQueueLoop(frame->data_, frame->data_size_);
            if (player->GetEffectType() == VideoRenderInterface::EffectType::kVisualAudio) {
                player->GetVisualAudioRender()->UpdateAudioFrame(frame);
            }
        }
    }
}

Player::Player(int view_type, int audio_render_type, int video_render_type, int effect_type,
               int scale_type) {
    view_type_ = static_cast<VideoRenderInterface::ViewType>(view_type);
    audio_render_type_ = static_cast<AudioRenderType>(audio_render_type);
    video_render_type_ = static_cast<VideoRenderInterface::VideoRenderType>(video_render_type);
    effect_type_ = static_cast<VideoRenderInterface::EffectType>(effect_type);
    scale_type_ = static_cast<ScaleType>(scale_type);
    demuxer_ = nullptr;
    audio_decoder_ = nullptr;
    audio_render_ = nullptr;
    opengles_render_ = nullptr;
    visual_audio_render_ = nullptr;
    is_inited_ = false;
}

Player::~Player() {}

bool Player::Init(const char *url) {
    TRACE_FUNC();

    int ret = false;

    demuxer_ = new FFmpegDemuxer(url);
    if (!demuxer_->Init()) {
        LOGE("FFmpegDemuxer init failed!");
        return ret;
    }

    audio_decoder_ = new FFmpegAudioDecoder(demuxer_);
    if (!audio_decoder_->Init()) {
        LOGE("FFmpegAudioDecorder init failed!");
        return ret;
    }

    ScaleFactory * scale_factory;
    if(scale_type_ == ScaleType::kFFmpeg) {
        scale_factory = new FFmpegScaleFactory();
    } else if(scale_type_ == ScaleType::kLibyuv) {
        scale_factory = new LibyuvScaleFactory();
    } else if(scale_type_ == ScaleType::kOpengles) {
        scale_factory = new OpenglScaleFactory();
    }
    video_decoder_ = new FFmpegVideoDecoder(demuxer_, scale_factory);
    if (!video_decoder_->Init()) {
        LOGE("FFmpegVideoDecoder init failed!");
        return ret;
    }

    audio_render_ = new SLRender();
    audio_render_->SetQueueCallBack(AudioRenderCallback);

    player = this;

    is_inited_ = true;

    return true;
}

void Player::Init2(JNIEnv *env, jobject obj, jobject surface) {
    env->GetJavaVM(&gl_jvm);
    gl_object = env->NewGlobalRef(obj);

    demuxer_->SetMessageCallback(this, PostMessage);


    if (video_render_type_ == VideoRenderInterface::VideoRenderType::kAnWindow) {
        video_render_ = new NativeWindowRender(env, surface, view_type_, video_render_type_,
                                               effect_type_);
    } else {
        OpenGLESRender *render = new OpenGLESRender(env, surface, view_type_, video_render_type_,
                                                    effect_type_);
        video_render_ = render;
        opengles_render_ = render;
    }

    int render_size[2] = {0};
    video_render_->Init(media_info_.width, media_info_.height, render_size);
    video_decoder_->SetRenderSize(render_size[0], render_size[1]);

    video_render_->SetMessageCallback(this, PostMessage);

    video_render_->SetAudioDecoder(audio_decoder_);
    video_render_->SetVideoDecoder(video_decoder_);
}

void Player::Uninit() {
    bool isAttach = false;
    GetJNIEnv(&isAttach)->DeleteGlobalRef(gl_object);
    if (isAttach)
        GetJavaVM()->DetachCurrentThread();
}

void Player::Start() {
    TRACE_FUNC();

    demuxer_->Start();

    audio_decoder_->Start();
    video_decoder_->Start();

    audio_render_->Start();
    audio_render_->SendQueueLoop("", 1);    // 开启轮询

    video_render_->Start();
}

FFmpegAudioDecoder *Player::GetAudioDecoder() const {
    return audio_decoder_;
};

SLBase *Player::GetAudioRender() const {
    return audio_render_;
}

void Player::PostMessage(void *context, int msgType, float msgCode) {
    // TRACE_FUNC();
    if (context != nullptr) {
        Player *player = static_cast<Player *>(context);
        bool is_attach = false;
        JNIEnv *env = player->GetJNIEnv(&is_attach);
        // LOGE("Player::PostMessage env=%p", env);
        if (env == nullptr)
            return;
        jobject javaObj = player->GetJavaObj();
        jmethodID mid = env->GetMethodID(env->GetObjectClass(javaObj),
                                         JAVA_PLAYER_EVENT_CALLBACK_API_NAME, "(IF)V");
        env->CallVoidMethod(javaObj, mid, msgType, msgCode);
        if (is_attach)
            player->GetJavaVM()->DetachCurrentThread();

    }
}

JNIEnv *Player::GetJNIEnv(bool *is_attach) {
    JNIEnv *env;
    int status;
    if (nullptr == gl_jvm) {
        LOGE("FFMediaPlayer::GetJNIEnv m_JavaVM == nullptr");
        return nullptr;
    }
    *is_attach = false;
    status = gl_jvm->GetEnv((void **) &env, JNI_VERSION_1_4);
    if (status != JNI_OK) {
        status = gl_jvm->AttachCurrentThread(&env, nullptr);
        if (status != JNI_OK) {
            LOGE("FFMediaPlayer::GetJNIEnv failed to attach current thread");
            return nullptr;
        }
        *is_attach = true;
    }
    return env;
}

jobject Player::GetJavaObj() {
    return gl_object;
}

JavaVM *Player::GetJavaVM() {
    return gl_jvm;
}

