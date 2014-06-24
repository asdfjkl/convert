#!/usr/bin/ruby -w

def bin_to_hex(s)
  s.each_byte.map { |b| b.to_s(16) }.join
end

Dir.glob('./KanjiTestData/*.xml') do |rb_file|
    file_out = File.open(rb_file+".new", "w:UTF-8")
    File.open(rb_file, "r:UTF-8") do |infile|
    while (line = infile.gets)
        if line =~ /unicode/
            matchData = line.match(/unicode>./)
            # ic = Iconv.new 'UTF-16', 'UTF-8'
            utf16string = ((matchData[0])[8]).encode("UTF-16")
            kanji_as_utf16 =  bin_to_hex(utf16string[2..3])
            file_out.write "<unicode>"
            file_out.write kanji_as_utf16
            file_out.write "</unicode>\n"
        else 
            file_out.write(line)# puts "nomatch
        end
    end
    # file_out.write
    file_out.close
end
end
