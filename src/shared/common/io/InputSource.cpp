
#include<common/io/InputSource.h>

#include<common/io/FileInputSource.h>
#include<common/io/HTTPInputSource.h>
#include<common/io/JARInputSource.h>

String *InputSource::getAbsolutePath(const String*basePath, const String*relPath){
  int root_pos = basePath->lastIndexOf('/');
  int root_pos2 = basePath->lastIndexOf('\\');
  if (root_pos2 > root_pos) root_pos = root_pos2;
  if (root_pos == -1) root_pos = 0;
  else root_pos++;
  StringBuffer *newPath = new StringBuffer();
  newPath->append(DString(basePath, 0, root_pos)).append(relPath);
  return newPath;
};

InputSource *InputSource::newInstance(const String *path){
  return newInstance(path, null);
};

InputSource *InputSource::newInstance(const String *path, InputSource *base){
  if (path == null) throw InputSourceException(DString("InputSource::newInstance: path is null"));
  if (path->startsWith(DString("http://"))){
    return new HTTPInputSource(path, null);
  };
  if (path->startsWith(DString("jar:"))){
    return new JARInputSource(path, base);
  };
  if (base != null){
    InputSource *is = base->createRelative(path);
    if (is != null) return is;
    throw InputSourceException(DString("Unknown input source type"));
  };
  return new FileInputSource(path, null);
};

bool InputSource::isRelative(const String *path){
  if (path->indexOf(':') != -1 && path->indexOf(':') < 10) return false;
  if (path->indexOf('/') == 0 || path->indexOf('\\') == 0) return false;
  return true;
};
