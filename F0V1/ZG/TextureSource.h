#pragma once
#include "Image.h"
#include "Variant.h"

namespace ZG
{
    struct TextureSource : Variant<std::string, Image, ID3D12Resource*>
    {
        ASAPI_VALUE_CLASS(TextureSource, asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_APP_CLASS_UNION);

        using Variant::Variant;

        ASAPI_CLASS_CONSTRUCTOR(
            <const std::string&>
            (t("const string& in filename")));

        // TODO
        // ASAPI_CLASS_CONSTRUCTOR(
        //     <Image>
        //     (t("const Image& in image")));
    };
}
