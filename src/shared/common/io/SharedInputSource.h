#ifndef _COLORER_SHAREDINPUTSOURCE_H_
#define _COLORER_SHAREDINPUTSOURCE_H_

#include<common/Common.h>

/** InputSource class wrapper,
    allows to manage class instances counter.
    @ingroup common_io
*/
class SharedInputSource : InputSource
{
public:
  SharedInputSource(const String *path, InputSource *base){
    is = InputSource::newInstance(path, base);
    stream = null;
    ref_count = 0;
  };
  ~SharedInputSource(){ delete is; };
  /** Increments reference counter */
  int addref(){  return ++ref_count; };
  /** Decrements reference counter */
  int delref(){  return --ref_count; };
  /** Returns currently opened stream.
      Opens it, if not yet opened.
  */
  const byte *getStream(){
    if (stream == null) openStream();
    return stream;
  };

  const String *getLocation() const{
    return is->getLocation();
  };
  const byte *openStream(){
    stream = is->openStream();
    return stream;
  };
  void closeStream(){ is->closeStream(); };
  int length() const{ return is->length(); };
private:
  InputSource *is;
  const byte *stream;
  int ref_count;
};

#endif