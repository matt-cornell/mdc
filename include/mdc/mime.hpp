#ifndef MIME_HPP
#define MIME_HPP
#include <string>
#include <unordered_map>
#include "debug.hpp"
namespace mdc {
  using namespace std::literals;
  inline const std::unordered_map<std::string_view, std::string> extensions {
    {"", "application/octet-stream"},
    {"aac", "audio/aac"},
    {"abw", "application/x-abiword"},
    {"arc", "application/x-freearc"},
    {"avif", "image/avif"},
    {"avi", "video/x-msvideo"},
    {"azw", "application/vnd.amazon.ebook"},
    {"bin", "application/octet-stream"},
    {"bmp", "image/bmp"},
    {"bz", "application/x-bzip"},
    {"bz2", "application/x-bzip2"},
    {"cda", "application/x-cdf"},
    {"csh", "application/x-csh"},
    {"css", "text/css"},
    {"csv", "text/csv"},
    {"doc", "application/msword"},
    {"docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {"eot", "application/vnd.ms-fontobject"},
    {"epub", "application/epub+zip"},
    {"gz", "application/gzip"},
    {"gif", "image/gif"},
    {"htm", "text/html"},
    {"html", "text/html"},
    {"ico", "application/vnd.microsoft.icon"},
    {"ics", "text/calendar"},
    {"jar", "application/java-archive"},
    {"jpeg", "image/jpeg"},
    {"jpg", "image/jpeg"},
    {"js", "text/javascript"},
    {"json", "application/json"},
    {"jsonld", "application/ld+json"},
    {"latex", "application/x-latex"},
    {"mid", "audio/midi"},
    {"midi", "audio/midi"},
    {"mjs", "text/javascript"},
    {"mp3", "audio/mpeg"},
    {"mp4", "video/mp4"},
    {"mpeg", "video/mpeg"},
    {"mpkg", "application/vnd.apple.installer+xml"},
    {"odp", "application/vnd.oasis.opendocument.presentation"},
    {"ods", "application/vnd.oasis.opendocument.spreadsheet"},
    {"odt", "application/vnd.oasis.opendocument.text"},
    {"oga", "audio/ogg"},
    {"ogv", "video/ogg"},
    {"ogx", "application/ogg"},
    {"opus", "audio/opus"},
    {"otf", "font/otf"},
    {"png", "image/png"},
    {"pdf", "application/pdf"},
    {"php", "application/x-httpd-php"},
    {"ppt", "application/vnd.ms-powerpoint"},
    {"pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
    {"rar", "application/vnd.rar"},
    {"rtf", "application/rtf"},
    {"sh", "application/x-sh"},
    {"svg", "image/svg+xml"},
    {"swf", "application/x-shockwave-flash"},
    {"tar", "application/x-tar"},
    {"tex", "application/x-latex"},
    {"tif", "image/tiff"},
    {"tiff", "image/tiff"},
    {"ts", "video/mp2t"},
    {"ttf", "font/ttf"},
    {"txt", "text/plain"},
    {"vsd", "application/vnd.visio"},
    {"wav", "audio/wav"},
    {"weba", "audio/webm"},
    {"webm", "video/webm"},
    {"webp", "image/webp"},
    {"woff", "font/woff"},
    {"woff2", "font/woff2"},
    {"xhtml", "application/xhtml+xml"},
    {"xls", "application/vnd.ms-excel"},
    {"xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {"xml", "application/xml"},
    {"xul", "application/vnd.mozilla.xul+xml"},
    {"zip", "application/zip"},
    {"3gp", "video/3gpp"},
    {"3g2", "video/3gpp2"},
    {"7z", "application/x-7z-compressed"}
  };
  const std::string& octet_stream = extensions.at("");
  std::string_view extension(std::string_view str) {
    auto idx = str.rfind('.');
    return idx == std::string::npos ? "" : str.substr(idx + 1);
  }
  const std::string& mime(const std::string& str) {
    auto it = extensions.find(extension(str));
    return it == extensions.end() ? octet_stream : it->second;
  }
  const std::string& mime(std::string_view str) {
    auto it = extensions.find(extension(str));
    return it == extensions.end() ? octet_stream : it->second;
  }
}
#endif // MIME_HPP
