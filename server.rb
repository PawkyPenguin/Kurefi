#!/usr/bin/env ruby

require 'socket'
require './KeyHandler.rb'
include Socket::Constants

Modifiers = ["shift", "lock", "ctrl", "alt", "mod2", "mod3", "mod4", "mod5", "button1", "button2", "button3", "button4", "button5"]

def main
	keyhandler = KeyHandler.new

	File.open('config', 'r') do |file|
		keyhandler.generate_methods file
	end
	# Delete old socket if we still have one 
	if File.exist?("./asdf")
		File.delete("./asdf")
	end
	# Open socket and start listening
	socket = Socket.new(AF_LOCAL, SOCK_SEQPACKET, 0)
	socket.bind(Socket.pack_sockaddr_un("./asdf"))
	socket.listen(10);
	while true
		client, client_addr = socket.accept
		# For every client, just stay attached until EOS
		while data = client.gets(" ")
			data.strip!
			if data.empty?
				next
			end
			puts "Received `#{data}`. Parse to event gives: `#{keyhandler.parse_to_event(data)}`"

			# determine method to handle our keystroke. These get generate from the config file
			handle_method = keyhandler.parse_to_event(data).to_sym
			# if our keyhandler has a method to handle our keystroke, execute it. Otherwise, just "forward" the keystroke by xdotool. This is ugly, but still better than parsing our config files in C to 
			# decide whether the server wants a keystroke or not...
			if KeyHandler.method_defined? handle_method
				keyhandler.send handle_method
			else
				system("xdotool key #{handle_method.to_s.split("_").join("+")}")
			end
		end

	end
end

main
