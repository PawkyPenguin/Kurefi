#!/usr/bin/env ruby

require 'socket'
include Socket::Constants


class KeyHandler
end

#module Parse
	def generate_methods(file)
		file.each do |line|
			line = line.chomp
			if line.empty?
				next
			end
			shortcut, command = line.split(':')
			key_parts = shortcut.tr(' ','').split("+")
			KeyHandler.class_eval {
				define_method("#{key_parts.join("_")}".to_sym) {
					puts "Executing #{command}"
					system("#{command}")
				}
			}
		end
	end
#end

File.open('config', 'r') do |file|
	#Parse::generate_methods file
	generate_methods file
end

handler = KeyHandler.new

if File.exist?("./asdf")
	File.delete("./asdf")
end
UNIXServer.open("./asdf") do |server|
	while true
		client = server.accept
		client.read.split("\n").each do |str|
			# I literally *cannot* check whether the string is empty, so we check the third character for nil-ness...
			if !str[2]
				next
			end
			if KeyHandler.method_defined? str
				handler.send "#{str}".to_sym
			end
		end

		client.close
	end
end
