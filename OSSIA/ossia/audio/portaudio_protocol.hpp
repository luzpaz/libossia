#pragma once
#if __has_include(<portaudio.h>)
#include <ossia/audio/audio_protocol.hpp>
#include <portaudio.h>

#if __has_include(<pa_jack.h>) && !defined(_MSC_VER)
#include <pa_jack.h>
#endif
namespace ossia
{
class portaudio_engine final
    : public audio_engine
{
  public:
    portaudio_engine()
    {
      if(Pa_Initialize() != paNoError)
        throw std::runtime_error("Audio error");
    }

    ~portaudio_engine() override
    {
      stop();
      Pa_Terminate();
    }

    void reload(ossia::audio_protocol* p) override
    {
      this->protocol = p;
      stop();

      if(!p)
        return;
      auto& proto = *p;
      proto.engine = this;
      auto& dev = proto.get_device();

#if __has_include(<pa_jack.h>) && !defined(_MSC_VER)
      PaJack_SetClientName(dev.get_name().c_str());
#endif
      int card_in_idx = -1;
      int card_out_idx = -1;

      for(int i = 0; i < Pa_GetDeviceCount(); i++)
      {
        auto raw_name = Pa_GetDeviceInfo(i)->name;
        if(raw_name == proto.card_in)
        {
          card_in_idx = i;
        }
        if(raw_name == proto.card_out)
        {
          card_out_idx = i;
        }
      }
      if(card_in_idx == -1 || card_out_idx == -1)
        return;

      auto devInInfo = Pa_GetDeviceInfo(card_in_idx);
      if(!devInInfo)
        return;
      auto devOutInfo = Pa_GetDeviceInfo(card_out_idx);
      if(!devOutInfo)
        return;

      proto.inputs = devInInfo->maxInputChannels;
      proto.outputs = devOutInfo->maxOutputChannels;

      proto.audio_ins.clear();
      proto.audio_outs.clear();

      proto.main_audio_in = ossia::net::find_or_create_parameter<ossia::audio_parameter>(dev.get_root_node(), "/in/main");
      proto.main_audio_out = ossia::net::find_or_create_parameter<ossia::audio_parameter>(dev.get_root_node(), "/out/main");
      for(int i = 0; i < proto.inputs; i++)
      {
        proto.audio_ins.push_back(ossia::net::find_or_create_parameter<ossia::audio_parameter>(dev.get_root_node(), "/in/" + std::to_string(i)));
      }
      for(int i = 0; i < proto.outputs; i++)
      {
        proto.audio_outs.push_back(ossia::net::find_or_create_parameter<ossia::audio_parameter>(dev.get_root_node(), "/out/" + std::to_string(i)));
      }

      proto.main_audio_in->audio.resize(proto.inputs);
      for(int i = 0; i < proto.inputs; i++)
      {
        proto.audio_ins[i]->audio.resize(1);
      }

      proto.main_audio_out->audio.resize(proto.outputs);
      for(int i = 0; i < proto.outputs; i++)
      {
        proto.audio_outs[i]->audio.resize(1);
      }


      PaStreamParameters inputParameters;
      inputParameters.device = card_in_idx;
      inputParameters.channelCount = proto.inputs;
      inputParameters.sampleFormat = paFloat32 | paNonInterleaved;
      inputParameters.suggestedLatency = 0.01;
      inputParameters.hostApiSpecificStreamInfo = nullptr;

      PaStreamParameters outputParameters;
      outputParameters.device = card_out_idx;
      outputParameters.channelCount = proto.outputs;
      outputParameters.sampleFormat = paFloat32 | paNonInterleaved;
      outputParameters.suggestedLatency = 0.01;
      outputParameters.hostApiSpecificStreamInfo = nullptr;

      std::cerr << "=== stream start ===\n";
      auto ec = Pa_OpenStream(&m_stream,
                              &inputParameters,
                              &outputParameters,
                              proto.rate,
                              proto.bufferSize, //paFramesPerBufferUnspecified,
                              paNoFlag,
                              &PortAudioCallback,
                              this);
      if(ec == PaErrorCode::paNoError)
      {
        ec = Pa_StartStream( m_stream );
        if(ec != PaErrorCode::paNoError)
        {
          std::cerr << "Error while starting audio stream: " << Pa_GetErrorText(ec) << std::endl;
        }
      }
      else
        std::cerr << "Error while opening audio stream: " << Pa_GetErrorText(ec) << std::endl;
    }

    void stop() override
    {
      if(m_stream)
      {
        auto ec = Pa_StopStream(m_stream);
        std::cerr << "=== stream stop ===\n";

        if(ec != PaErrorCode::paNoError)
        {
          std::cerr << "Error while stopping audio stream: " << Pa_GetErrorText(ec) << std::endl;
        }

        m_stream = nullptr;
      }
    }

  private:
    static int PortAudioCallback(
        const void* input,
        void* output,
        unsigned long frameCount,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData)
    {
      auto& self = *static_cast<portaudio_engine*>(userData);
      auto float_input = ((float **) input);
      auto float_output = ((float **) output);

      self.protocol.load()->process_generic(*self.protocol.load(), float_input, float_output, frameCount);
      self.protocol.load()->audio_tick(frameCount, timeInfo->currentTime);

      return paContinue;
    }

    PaStream* m_stream{};
};

}

#endif