@c_scanned = {}
@m_scanned = {}
@v_scanned = {}

def gen(klass)
  @c_scanned[klass.to_s] = 1

  klass.public_instance_methods(false).each do |m|
    next if m.length < 3
    @m_scanned[m] = 1
    # klass.to_s + "." + 
  end

  klass.public_methods(false).each do |m|
    next if m.length < 3
    @m_scanned[m] = 1
  end

  #klass.public_methods.each do |m|
  #  @m_scanned[m] = 1
  #end

end


#Kernel.public_methods.each{|m| push m}


ObjectSpace.each_object(Class) do |klass|

  gen klass

end

@c_scanned.each_key do |c|
  puts "        <word name='#{c}'/>"
end

2.times { puts }

@m_scanned.each_key do |m|
  puts "        <symb name='.#{m}'/>"
end

2.times { puts }

@v_scanned.each_key do |v|
  puts "    <word name='%{v}'/>"
end
