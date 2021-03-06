//
// # Yocto/GLIO: Inpuot and output functions for Yocto/GL
//
// Yocto/GLIO provides loading and saving functionality for images and scenes
// in Yocto/GL. For images we support PNG, JPG, TGA, HDR, EXR formats. For
// scene we support a simple to use JSON format, PLY, OBJ and glTF.
// The JSON serialization is a straight copy of the in-memory scene data.
// To speed up testing, we also support a binary format that is a dump of
// the current scene. This format should not be use for archival though.
//
// We do not use exception as the API for returning errors, although they might
// be used internally in the implementastion of the methods. In load functions,
// as error is signaled by returning an empty object or a null pointer. In
// save functions, errors are returned with the supplied boolean. In the future,
// we will also provide return types with error codes.
//
// ## File Loading and Saving
//
// 1. manipulate paths withe path utilities
// 2. load and save text files with `load_text()` and `save_text()`
// 3. load and save binary files with `load_binary()` and `save_binary()`
// 4. load and save images with `load_image4f()` and `save_image4f()`
// 5. load a scene with `load_json_scene()` and save it with `save_json_scene()`
// 6. load and save OBJs with `load_obj_scene()` and `save_obj_scene()`
// 7. load and save glTFs with `load_gltf_scene()` and `save_gltf_scene()`
// 8. load and save binary dumps with `load_ybin_scene()`and `save_ybin_scene()`
// 9. if desired, the function `load_scene()` and `save_scene()` will either
//    load using the internal format or convert on the fly using on the
//    supported conversions
//
//
// ## Command-Line Parsing
//
// We also provide a simple, immediate-mode, command-line parser. The parser
// works in an immediate-mode manner since it reads each value as you call each
// function, rather than building a data structure and parsing offline. We
// support option and position arguments, automatic help generation, and
// error checking.
//
// 1. initialize the parser with `make_cmdline_parser(argc, argv, help)`
// 2. read a value with `value = parse_arg(parser, name, default, help)`
//    - is name starts with '--' or '-' then it is an option
//    - otherwise it is a positional arguments
//    - options and arguments may be intermixed
//    - the type of each option is determined by the default value `default`
//    - the value is parsed on the stop
// 3. finished parsing with `check_cmdline(parser)`
//    - if an error occurred, the parser will exit and print a usage message
//
//

//
// LICENSE:
//
// Copyright (c) 2016 -- 2018 Fabio Pellacini
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef _YGLIO_H_
#define _YGLIO_H_

#include "ygl.h"

// use fast parsing in OBJ
#ifndef YGL_FASTPARSE
#define YGL_FASTPARSE 1
#endif

// use Happly library for PLY format rather than our library
#ifndef YGL_HAPPLY
#define YGL_HAPPLY 1
#endif

// -----------------------------------------------------------------------------
// PATH UTILITIES
// -----------------------------------------------------------------------------
namespace ygl {

// Normalize path delimiters.
string normalize_path(const string& filename);
// Get directory name (not including '/').
string get_dirname(const string& filename);
// Get extension (not including '.').
string get_extension(const string& filename);
// Get filename without directory.
string get_filename(const string& filename);
// Replace extension.
string replace_extension(const string& filename, const string& ext);

// Check if a file can be opened for reading.
bool exists_file(const string& filename);

}  // namespace ygl

// -----------------------------------------------------------------------------
// TRIVIAL COMMAND LINE PARSING
// -----------------------------------------------------------------------------
namespace ygl {

// Command line parser data. All data should be considered private.
struct cmdline_parser {
    vector<string> args      = {};  // command line arguments
    string         usage_cmd = "";  // program name
    string         usage_hlp = "";  // program help
    string         usage_opt = "";  // options help
    string         usage_arg = "";  // arguments help
    string         error     = "";  // current parse error
};

// Initialize a command line parser.
cmdline_parser make_cmdline_parser(
    int argc, char** argv, const string& usage, const string& cmd = "");
// check if any error occurred and exit appropriately
void check_cmdline(cmdline_parser& parser);

// Parse an int, float, string, vecXX and bool option or positional argument.
// Options's names starts with "--" or "-", otherwise they are arguments.
// vecXX options use space-separated values but all in one argument
// (use " or ' from the common line). Booleans are flags.
template <typename T>
inline T      parse_arg(cmdline_parser& parser, const string& name, T def,
         const string& usage, bool req = false);
inline string parse_arg(cmdline_parser& parser, const string& name,
    const char* def, const string& usage, bool req = false);
// Parse all arguments left on the command line.
template <typename T>
inline vector<T> parse_args(cmdline_parser& parser, const string& name,
    const vector<T>& def, const string& usage, bool req = false);
// Parse a labeled enum, with enum values that are successive integers.
template <typename T>
inline T parse_arge(cmdline_parser& parser, const string& name, T def,
    const string& usage, const vector<string>& labels, bool req = false);

}  // namespace ygl

// -----------------------------------------------------------------------------
// FILE IO
// -----------------------------------------------------------------------------
namespace ygl {

// Load/save a text file
string load_text(const string& filename);
bool   save_text(const string& filename, const string& str);

// Load/save a binary file
vector<byte> load_binary(const string& filename);
bool         save_binary(const string& filename, const vector<byte>& data);

}  // namespace ygl

// -----------------------------------------------------------------------------
// IMAGE IO
// -----------------------------------------------------------------------------
namespace ygl {

// Check if an image is HDR based on filename.
bool is_hdr_filename(const string& filename);

// Loads/saves a 4 channel float image in linear color space.
image<vec4f> load_image4f(const string& filename);
bool         save_image4f(const string& filename, const image<vec4f>& img);
image<vec4f> load_image4f_from_memory(const byte* data, int data_size);

// Loads/saves a 4 channel byte image in sRGB color space.
image<vec4b> load_image4b(const string& filename);
bool         save_image4b(const string& filename, const image<vec4b>& img);
bool load_image4b_from_memory(const byte* data, int data_size, image<vec4b>& img);
image<vec4b> load_image4b_from_memory(const byte* data, int data_size);

// Load 4 channel images with shortened api. Returns empty image on error;

// Convenience helper that saves an HDR images as wither a linear HDR file or
// a tonemapped LDR file depending on file name
bool save_tonemapped_image(const string& filename, const image<vec4f>& hdr,
    float exposure = 0, bool filmic = false, bool srgb = true);

}  // namespace ygl

// -----------------------------------------------------------------------------
// VOLUME IMAGE IO
// -----------------------------------------------------------------------------
namespace ygl {

// Loads/saves a 1 channel volume.
volume<float> load_volume1f(const string& filename);
bool          save_volume1f(const string& filename, const volume<float>& vol);

}  // namespace ygl

// -----------------------------------------------------------------------------
// SCENE IO FUNCTIONS
// -----------------------------------------------------------------------------
namespace ygl {

// Load/save a scene in the supported formats.
yocto_scene* load_scene(const string& filename, bool load_textures = true,
    bool skip_missing = true);
bool         save_scene(const string& filename, const yocto_scene* scene,
            bool save_textures = true, bool skip_missing = true);

// Load/save a scene in the builtin JSON format.
yocto_scene* load_json_scene(const string& filename, bool load_textures = true,
    bool skip_missing = true);
bool         save_json_scene(const string& filename, const yocto_scene* scene,
            bool save_textures = true, bool skip_missing = true);

// Load/save a scene from/to OBJ.
yocto_scene* load_obj_scene(const string& filename, bool load_textures = true,
    bool skip_missing = true, bool split_shapes = true,
    bool preserve_face_varying = true);
bool         save_obj_scene(const string& filename, const yocto_scene* scene,
            bool save_textures = true, bool skip_missing = true);

// Load/save a scene from/to glTF.
yocto_scene* load_gltf_scene(const string& filename, bool load_textures = true,
    bool skip_missing = true);
bool         save_gltf_scene(const string& filename, const yocto_scene* scene,
            bool save_textures = true, bool skip_missing = true);

// Load/save a scene from/to pbrt. This is not robust at all and only
// works on scene that have been previously adapted since the two renderers
// are too different to match.
yocto_scene* load_pbrt_scene(const string& filename, bool load_textures = true,
    bool skip_missing = true);
bool         save_pbrt_scene(const string& filename, const yocto_scene* scene,
            bool save_textures = true, bool skip_missing = true);

// Load/save a binary dump useful for very fast scene IO. This format is not
// an archival format and should only be used as an intermediate format.
yocto_scene* load_ybin_scene(const string& filename, bool load_textures = true,
    bool skip_missing = true);
bool         save_ybin_scene(const string& filename, const yocto_scene* scene,
            bool save_textures = true, bool skip_missing = true);

}  // namespace ygl

// -----------------------------------------------------------------------------
// MESH IO FUNCTIONS
// -----------------------------------------------------------------------------
namespace ygl {

// Load/Save a mesh
bool load_mesh(const string& filename, vector<int>& points,
    vector<vec2i>& lines, vector<vec3i>& triangles, vector<vec4i>& quads,
    vector<vec4i>& quads_positions, vector<vec4i>& quads_normals,
    vector<vec4i>& quads_texturecoords, vector<vec3f>& positions,
    vector<vec3f>& normals, vector<vec2f>& texturecoords, vector<vec4f>& colors,
    vector<float>& radius, bool force_triangles);
bool save_mesh(const string& filename, const vector<int>& points,
    const vector<vec2i>& lines, const vector<vec3i>& triangles,
    const vector<vec4i>& quads, const vector<vec4i>& quads_positions,
    const vector<vec4i>& quads_normals, const vector<vec4i>& quads_texturecoords,
    const vector<vec3f>& positions, const vector<vec3f>& normals,
    const vector<vec2f>& texturecoords, const vector<vec4f>& colors,
    const vector<float>& radius, bool ascii = false);

// Load/Save a ply mesh
bool load_ply_mesh(const string& filename, vector<int>& points,
    vector<vec2i>& lines, vector<vec3i>& triangles, vector<vec4i>& quads,
    vector<vec4i>& quads_positions, vector<vec4i>& quads_normals,
    vector<vec4i>& quads_texturecoords, vector<vec3f>& positions,
    vector<vec3f>& normals, vector<vec2f>& texturecoords, vector<vec4f>& color,
    vector<float>& radius, bool force_triangles);
bool save_ply_mesh(const string& filename, const vector<int>& points,
    const vector<vec2i>& lines, const vector<vec3i>& triangles,
    const vector<vec4i>& quads, const vector<vec4i>& quads_positions,
    const vector<vec4i>& quads_normals, const vector<vec4i>& quads_texturecoords,
    const vector<vec3f>& positions, const vector<vec3f>& normals,
    const vector<vec2f>& texturecoords, const vector<vec4f>& colors,
    const vector<float>& radius, bool ascii = false);

// Load/Save an OBJ mesh
bool load_obj_mesh(const string& filename, vector<int>& points,
    vector<vec2i>& lines, vector<vec3i>& triangles, vector<vec4i>& quads,
    vector<vec4i>& quads_positions, vector<vec4i>& quads_normals,
    vector<vec4i>& quads_texturecoords, vector<vec3f>& positions,
    vector<vec3f>& normals, vector<vec2f>& texturecoords, bool force_triangles,
    bool preserve_faceavarying = true, bool flip_texcoord = true);
bool save_obj_mesh(const string& filename, const vector<int>& points,
    const vector<vec2i>& lines, const vector<vec3i>& triangles,
    const vector<vec4i>& quads, const vector<vec4i>& quads_positions,
    const vector<vec4i>& quads_normals, const vector<vec4i>& quads_texturecoords,
    const vector<vec3f>& positions, const vector<vec3f>& normals,
    const vector<vec2f>& texturecoords, bool flip_texcoord = true);

}  // namespace ygl

// -----------------------------------------------------------------------------
// SIMPLE OBJ LOADER
// -----------------------------------------------------------------------------
namespace ygl {

// OBJ vertex
struct obj_vertex {
    int position     = 0;
    int texturecoord = 0;
    int normal       = 0;
};

// Obj texture information.
struct obj_texture_info {
    string path  = "";     // file path
    bool   clamp = false;  // clamp to edge
    float  scale = 1;      // scale for bump/displacement
    // Properties not explicitly handled.
    unordered_map<string, vector<float>> props;
};

// Obj material.
struct obj_material {
    string name;       // name
    int    illum = 0;  // MTL illum mode

    // base values
    vec3f ke  = {0, 0, 0};  // emission color
    vec3f ka  = {0, 0, 0};  // ambient color
    vec3f kd  = {0, 0, 0};  // diffuse color
    vec3f ks  = {0, 0, 0};  // specular color
    vec3f kr  = {0, 0, 0};  // reflection color
    vec3f kt  = {0, 0, 0};  // transmission color
    float ns  = 0;          // Phong exponent color
    float ior = 1;          // index of refraction
    float op  = 1;          // opacity
    float rs  = -1;         // roughness (-1 not defined)
    float km  = -1;         // metallic  (-1 not defined)

    // textures
    obj_texture_info ke_txt;    // emission texture
    obj_texture_info ka_txt;    // ambient texture
    obj_texture_info kd_txt;    // diffuse texture
    obj_texture_info ks_txt;    // specular texture
    obj_texture_info kr_txt;    // reflection texture
    obj_texture_info kt_txt;    // transmission texture
    obj_texture_info ns_txt;    // Phong exponent texture
    obj_texture_info op_txt;    // opacity texture
    obj_texture_info rs_txt;    // roughness texture
    obj_texture_info km_txt;    // metallic texture
    obj_texture_info ior_txt;   // ior texture
    obj_texture_info occ_txt;   // occlusion map
    obj_texture_info bump_txt;  // bump map
    obj_texture_info disp_txt;  // displacement map
    obj_texture_info norm_txt;  // normal map

    // volume data [extension]
    vec3f ve = {0, 0, 0};  // volume emission
    vec3f va = {0, 0, 0};  // albedo: scattering / (absorption + scattering)
    vec3f vd = {0, 0, 0};  // density: absorption + scattering
    float vg = 0;          // phase function shape

    // volume textures [extension]
    obj_texture_info vd_txt;  // density

    // Properties not explicitly handled.
    unordered_map<string, vector<string>> props;
};

// Obj camera [extension].
struct obj_camera {
    string  name;                         // name
    frame3f frame    = identity_frame3f;  // transform
    bool    ortho    = false;             // orthographic
    vec2f   film     = {0.036f, 0.024f};  // film size (default to 35mm)
    float   focal    = 0.050f;            // focal length
    float   aspect   = 16.0f / 9.0f;      // aspect ratio
    float   aperture = 0;                 // lens aperture
    float   focus    = maxf;              // focus distance
};

// Obj environment [extension].
struct obj_environment {
    string           name;                      // name
    frame3f          frame = identity_frame3f;  // transform
    vec3f            ke    = zero3f;            // emission color
    obj_texture_info ke_txt;                    // emission texture
};

// Obj callbacks
struct obj_callbacks {
    function<void(const vec3f&)>              vert        = {};
    function<void(const vec3f&)>              norm        = {};
    function<void(const vec2f&)>              texcoord    = {};
    function<void(const vector<obj_vertex>&)> face        = {};
    function<void(const vector<obj_vertex>&)> line        = {};
    function<void(const vector<obj_vertex>&)> point       = {};
    function<void(const string& name)>        object      = {};
    function<void(const string& name)>        group       = {};
    function<void(const string& name)>        usemtl      = {};
    function<void(const string& name)>        smoothing   = {};
    function<void(const string& name)>        mtllib      = {};
    function<void(const obj_material&)>       material    = {};
    function<void(const obj_camera&)>         camera      = {};
    function<void(const obj_environment&)>    environmnet = {};
};

// Load obj scene
bool load_obj(const string& filename, const obj_callbacks& cb,
    bool geometry_only = false, bool skip_missing = true,
    bool flip_texcoord = true, bool flip_tr = true);

}  // namespace ygl

// -----------------------------------------------------------------------------
// SIMPLE PLY LOADER
// -----------------------------------------------------------------------------
namespace ygl {

// ply type
enum struct ply_type { ply_uchar, ply_int, ply_float, ply_int_list };

// ply property
struct ply_property {
    string                name    = "";
    ply_type              type    = ply_type::ply_float;
    vector<float>         scalars = {};
    vector<array<int, 8>> lists   = {};
};

// ply element
struct ply_element {
    string               name       = "";
    int                  count      = 0;
    vector<ply_property> properties = {};
};

// simple ply api data
struct ply_data {
    vector<ply_element> elements = {};
};

// Load ply mesh
ply_data* load_ply(const string& filename);

}  // namespace ygl

// -----------------------------------------------------------------------------
// IMPLEMENTATION OF COMMAND-LINE PARSING
// -----------------------------------------------------------------------------
namespace ygl {

// initialize a command line parser
inline cmdline_parser make_cmdline_parser(
    int argc, char** argv, const string& usage, const string& cmd) {
    auto parser      = cmdline_parser();
    parser.args      = {argv + 1, argv + argc};
    parser.usage_cmd = (cmd.empty()) ? argv[0] : cmd;
    parser.usage_hlp = usage;
    return parser;
}

// check if option or argument
inline bool is_option(const string& name) {
    return name.size() > 1 && name.front() == '-';
}

// get names from string
inline vector<string> get_option_names(const string& name_) {
    auto names = vector<string>();
    auto name  = name_;
    while (name.find(',') != name.npos) {
        names.push_back(name.substr(0, name.find(',')));
        name = name.substr(name.find(',') + 1);
    }
    names.push_back(name);
    return names;
}

// add help
inline string get_option_usage(const string& name, const string& var,
    const string& usage, const string& def_, const vector<string>& choices) {
    auto def = def_;
    if (def != "") def = "[" + def + "]";
    auto namevar = name;
    if (var != "") namevar += " " + var;
    char buf[4096];
    sprintf(buf, "  %-24s %s %s\n", namevar.c_str(), usage.c_str(), def.c_str());
    auto usagelines = string(buf);
    if (!choices.empty()) {
        usagelines += "        accepted values:";
        for (auto& c : choices) usagelines += " " + c;
        usagelines += "\n";
    }
    return usagelines;
}

// print cmdline help
inline void print_cmdline_usage(const cmdline_parser& parser) {
    printf("%s: %s\n", parser.usage_cmd.c_str(), parser.usage_hlp.c_str());
    printf("usage: %s %s %s\n\n", parser.usage_cmd.c_str(),
        (parser.usage_opt.empty()) ? "" : "[options]",
        (parser.usage_arg.empty()) ? "" : "arguments");
    if (!parser.usage_opt.empty()) {
        printf("options:\n");
        printf("%s\n", parser.usage_opt.c_str());
    }
    if (!parser.usage_arg.empty()) {
        printf("arguments:\n");
        printf("%s\n", parser.usage_arg.c_str());
    }
}

// Parse a flag. Name should start with either "--" or "-".
inline bool parse_flag(
    cmdline_parser& parser, const string& name, bool def, const string& usage);

// check if any error occurred and exit appropriately
inline void check_cmdline(cmdline_parser& parser) {
    if (parse_flag(parser, "--help,-?", false, "print help")) {
        print_cmdline_usage(parser);
        exit(0);
    }
    if (!parser.args.empty()) parser.error += "unmatched arguments remaining\n";
    if (!parser.error.empty()) {
        printf("error: %s", parser.error.c_str());
        print_cmdline_usage(parser);
        exit(1);
    }
}

// Parse an option string. Name should start with "--" or "-".
template <typename T>
inline T parse_option(cmdline_parser& parser, const string& name, T def,
    const string& usage, bool req, const vector<string>& choices) {
    parser.usage_opt += get_option_usage(
        name, "", usage, to_string(def), choices);
    if (parser.error != "") return def;
    auto names = get_option_names(name);
    auto pos   = parser.args.end();
    for (auto& name : names) {
        pos = std::min(
            pos, std::find(parser.args.begin(), parser.args.end(), name));
    }
    if (pos == parser.args.end()) {
        if (req) parser.error += "missing value for " + name;
        return def;
    }
    if (pos == parser.args.end() - 1) {
        parser.error += "missing value for " + name;
        return def;
    }
    auto vals = *(pos + 1);
    parser.args.erase(pos, pos + 2);
    if (!choices.empty() &&
        std::find(choices.begin(), choices.end(), vals) == choices.end()) {
        parser.error += "bad value for " + name;
        return def;
    }
    auto val = def;
    if (!parse(vals, val)) {
        parser.error += "bad value for " + name;
        return def;
    }
    return val;
}

// Parse an argument string. Name should not start with "--" or "-".
template <typename T>
inline T parse_argument(cmdline_parser& parser, const string& name, const T def,
    const string& usage, bool req, const vector<string>& choices) {
    parser.usage_arg += get_option_usage(
        name, "", usage, to_string(def), choices);
    if (parser.error != "") return def;
    auto pos = std::find_if(parser.args.begin(), parser.args.end(),
        [](auto& v) { return v[0] != '-'; });
    if (pos == parser.args.end()) {
        if (req) parser.error += "missing value for " + name;
        return def;
    }
    auto vals = *pos;
    parser.args.erase(pos);
    if (!choices.empty() &&
        std::find(choices.begin(), choices.end(), vals) == choices.end()) {
        parser.error += "bad value for " + name;
        return def;
    }
    auto val = def;
    if (!parse(vals, val)) {
        parser.error += "bad value for " + name;
        return def;
    }
    return val;
}

// Parse all left argument strings. Name should not start with "--" or "-".
template <typename T>
inline vector<T> parse_arguments(cmdline_parser& parser, const string& name,
    const vector<T>& def, const string& usage, bool req) {
    auto defs = string();
    for (auto& d : def) defs += " " + d;
    parser.usage_arg += get_option_usage(name, "", usage, defs, {});
    if (parser.error != "") return {};
    auto pos = std::find_if(parser.args.begin(), parser.args.end(),
        [](auto& v) { return v[0] != '-'; });
    if (pos == parser.args.end()) {
        if (req) parser.error += "missing value for " + name;
        return {};
    }
    auto val = vector<string>{pos, parser.args.end()};
    parser.args.erase(pos, parser.args.end());
    return val;
}

// Parse a flag. Name should start with either "--" or "-".
inline bool parse_flag(
    cmdline_parser& parser, const string& name, bool def, const string& usage) {
    parser.usage_opt += get_option_usage(name, "", usage, "", {});
    if (parser.error != "") return def;
    auto names = get_option_names(name);
    auto pos   = parser.args.end();
    for (auto& name : names)
        pos = std::min(
            pos, std::find(parser.args.begin(), parser.args.end(), name));
    if (pos == parser.args.end()) return def;
    parser.args.erase(pos);
    return !def;
}

// Parse an integer, float, string. If name starts with "--" or "-", then it is
// an option, otherwise it is a position argument.
template <typename T>
inline T parse_arg(cmdline_parser& parser, const string& name, T def,
    const string& usage, bool req) {
    return is_option(name) ? parse_option(parser, name, def, usage, req, {}) :
                             parse_argument(parser, name, def, usage, req, {});
}
template <>
inline bool parse_arg<bool>(cmdline_parser& parser, const string& name,
    bool def, const string& usage, bool req) {
    return parse_flag(parser, name, def, usage);
}

template <typename T>
inline T parse_arge(cmdline_parser& parser, const string& name, T def,
    const string& usage, const vector<string>& labels, bool req) {
    auto val = is_option(name) ? parse_option(parser, name, labels.at((int)def),
                                     usage, req, labels) :
                                 parse_argument(parser, name,
                                     labels.at((int)def), usage, req, labels);
    return (T)(std::find(labels.begin(), labels.end(), val) - labels.begin());
}

// Parser an argument
template <typename T>
inline vector<T> parse_args(cmdline_parser& parser, const string& name,
    const vector<T>& def, const string& usage, bool req) {
    return parse_arguments(parser, name, def, usage, req);
}

// Override to avoid issues with const char
inline string parse_arg(cmdline_parser& parser, const string& name,
    const char* def, const string& usage, bool req) {
    return parse_arg(parser, name, std::string(def), usage, req);
}

}  // namespace ygl

#endif
