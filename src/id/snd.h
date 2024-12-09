#ifndef _KARIN_SNDSHADER_H
#define _KARIN_SNDSHADER_H

#include <iostream>

#include "str.h"
#include "list.h"
#include "dict.h"

class idSndShader
{
public:
    enum SOUNDCLASS {
        SOUNDCLASS_NORMAL		= 0,
        SOUNDCLASS_VOICEDUCKER,
        // SOUNDCLASS_SPIRITWALK	= 2,
        SOUNDCLASS_VOICE,
        SOUNDCLASS_MUSIC,
    };
    static const char *TYPE_NAME;

	public:
		idSndShader();
		idStr & Name();
		const idStr & Name() const;
        void SetSoundFile(const char *sf);
		void SetMinDistance(float minDistance);
		void SetMaxDistance(float maxDistance);
		void SetVolume(float volume);
		void SetSoundClass(int soundclass);
		void Global(bool b = true);
		void Looping(bool b = true);
        void Plain(bool b = true);
        void NoOcclusion(bool b = true);
		idSndShader & operator()(const char *name, const char *value = "");

		friend std::ostream & operator<<(std::ostream &o, const idSndShader &v);

	private:
		idStr name;
        idStr soundFile;
        float minDistance = 0.0f;
        float maxDistance = 10.0f;
        float volume = 1.0;
        int soundClass = SOUNDCLASS_NORMAL;
        bool global = false;
        bool looping = false;
        bool plain = false;
        bool noOcclusion = false;
		idDict parms;
};
using idSndShaderList = idList<idSndShader>;



inline idStr & idSndShader::Name()
{
	return name;
}

inline const idStr & idSndShader::Name() const
{
	return name;
}

inline void idSndShader::SetSoundFile(const char *sf)
{
    this->soundFile = sf;
}

inline void idSndShader::SetMinDistance(float f)
{
	this->minDistance = f;
}

inline void idSndShader::SetMaxDistance(float f)
{
	this->maxDistance = f;
}
inline void idSndShader::SetVolume(float f)
{
    this->volume = f;
}

inline void idSndShader::SetSoundClass(int c)
{
    this->soundClass = c;
}

inline void idSndShader::Global(bool b)
{
    this->global = b;
}

inline void idSndShader::Looping(bool b)
{
    this->looping = b;
}

inline void idSndShader::Plain(bool b)
{
    this->plain = b;
}

inline void idSndShader::NoOcclusion(bool b)
{
    this->noOcclusion = b;
}


inline idSndShader::idSndShader()
{
	parms.NoQuota(true);
}

inline idSndShader & idSndShader::operator()(const char *name, const char *value)
{
	parms.Set(name, value);
	return *this;
}

#endif
