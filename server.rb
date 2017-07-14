#!/usr/bin/env ruby

require 'socket'
include Socket::Constants


class KeyHandler
end

module Parse
	def generate_methods(file)
		file.each do |line|
			line.chomp!.tr!(' ', '')
			if line.empty?
				next
			end
			shortcut, command = line.split(':')
			key_parts = shortcut.split("+")


		end
	end
end

#File.open('config', 'r') do |file|
	#Parse::generate_mehods file
#
#end

if File.exist?("./asdf")
	File.delete("./asdf")
end
UNIXServer.open("./asdf") do |server|
	while true
		client = server.accept
		p client.read

		client.close
	end
end
