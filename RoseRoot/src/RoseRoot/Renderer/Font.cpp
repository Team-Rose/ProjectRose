#include "rrpch.h"
#include "Font.h"

#undef INFINITE
#include "msdf-atlas-gen.h"

namespace Rose {
	Font::Font(const std::filesystem::path& filepath)
	{
        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
        if (ft) {
            std::string filString = filepath.string();
            msdfgen::FontHandle* font = loadFont(ft, filString.c_str());
            if (font) {
                msdfgen::Shape shape;
                if (loadGlyph(shape, font, 'S')) {
                    shape.normalize();
                    //                      max. angle
                    edgeColoringSimple(shape, 3.0);

                    //           image width, height
                    msdfgen::Bitmap<float, 3> msdf(32, 32);

                    //                     range, scale, translation
                    generateMSDF(msdf, shape, 4.0, 1.0, msdfgen::Vector2(4.0, 4.0));
                    savePng(msdf, "output.png");
                }
                destroyFont(font);
            }
            deinitializeFreetype(ft);
        }
	}
}