import os,re,string,sys,popen2
Dependencies = ["time"]

class Parser:
    def __init__(self, raw, request, **kw):
        # save call arguments for later use in format
        self.raw = raw
        self.request = request
        self.kw= kw

    def format(self, formatter):
        #type to pass to colorer
        type=string.strip(self.kw['format_args'])
        options='-dc -dh -h -t%s' % type
        cmd = 'colorer ' + options
        try:
            fromchild, tochild, errchild = popen2.popen3(cmd)
            tochild.write(self.raw)
            tochild.flush()
            tochild.close()
            fromchild.flush()
            html = fromchild.readlines()
        finally:
            fromchild.close()
            errchild.close()

        html='<PRE>'+string.join(html, '')+'</PRE>'

        self.request.write(formatter.rawHTML(html))



def foo(bar)
  bar.gsub(%r{^\.|[\s/\\\*\:\?'"<>\|]}, '_')
end