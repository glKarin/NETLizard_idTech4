#include "snd.h"

const char *idSndShader::TYPE_NAME = "sound";

std::ostream & operator<<(std::ostream &o, const idSndShader &v)
{
	o << idSndShader::TYPE_NAME << " " << v.name << "\n";
	o << "{\n";

	if(v.minDistance >= 0.0f)
		o << "\tminDistance " << v.minDistance << "\n";
    if(v.maxDistance >= 0.0f)
        o << "\tmaxDistance " << v.maxDistance << "\n";
    if(v.volume >= 0.0f)
        o << "\tvolume " << v.volume << "\n";
	if(v.soundClass >= 0)
        o << "\tsoundClass " << v.soundClass << "\n";
	if(v.global)
		o << "\tglobal\n";
	if(v.looping)
		o << "\tlooping\n";
	if(v.plain)
		o << "\tplain\n";
    if(v.noOcclusion)
        o << "\tno_occlusion\n";

    if(!v.parms.IsEmpty())
        o << v.parms;

    o << "\t" << v.soundFile.c_str() << "\n";

	o << "}\n";
	return o;
}
