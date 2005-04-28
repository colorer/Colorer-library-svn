#include<common/io/SharedInputSource.h>

Hashtable<SharedInputSource*> SharedInputSource::isHash;


SharedInputSource::SharedInputSource(InputSource *source){
  is = source;
  stream = null;
  ref_count = 1;
}

SharedInputSource::~SharedInputSource(){
  isHash.remove(is->getLocation());
  delete is;
}


SharedInputSource *SharedInputSource::getInputSource(const String *path, InputSource *base)
{
  InputSource *tempis = InputSource::newInstance(path, base);

  SharedInputSource *sis = isHash.get(tempis->getLocation());

  if (sis == null){
    sis = new SharedInputSource(tempis);
    isHash.put(tempis->getLocation(), sis);
    return sis;
  }else{
    delete tempis;
  }

  sis->addref();
  return sis;
}
