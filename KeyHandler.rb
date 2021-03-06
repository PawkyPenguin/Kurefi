#!/usr/bin/env ruby
class KeyHandler
	# Parse the config file to generate all methods for the keyhandler.
	# The config file has the syntax `modifiers shortcut : command`, where `modifiers` is something along
	# the lines of `shift + ctrl` or `ctrl`, or simply an empty string. `shortcut` is 
	def generate_methods(file)
		file.each_with_index do |line, i|
			line = line.strip
			if line.empty?
				next
			end

			shortcut, command = line.split(':')
			if !shortcut || !command
				abort("Line does not contain a ':'")
			end
			if shortcut.strip.empty?
				abort("Error: The shortcut on line #{i + 1} is empty.")
			end
			if !command or command.strip.empty?
				puts "Warning: The command for shortcut `#{shortcut}` on line #{i + 1} is empty."
			end

			# First, split modifiers and the actual key. `modifiers` becomes an array of strings.
			*modifiers, key = shortcut.tr(' ','').downcase.split("+")
			if modifiers.any? {|e| !Modifiers.member? e}
				abort("Error: Unknown modifier on line #{i + 1}. Probably you made a typo.")
			end
			if key.strip.empty?
				abort("Error: No key for shortcut `#{shortcut}` on line #{i + 1}")
			end
			# we want our modifiers to be sorted in the same order as in the Modifiers constant
			modifiers.sort_by! {|e| Modifiers.index e}

			# Finally, define a method with the name of the keyshortcut. Note that a keyshortcut without modifier
			# becomes a method that starts with `_`. This is so we can differentiate between user-defined events and keypresses.
			method_name = modifiers.join("_") + "_" + key
			puts "Defining shortcut: #{shortcut} with name #{method_name}. Modifiers: #{modifiers}, key: #{key}"
			self.class.class_eval {
				define_method(method_name.to_sym) {
					puts "Executing #{command}"
					system("#{command}")
				}
			}
		end
	end

	# Parse string received by clients. If the string has a `_` prepended in front of it, it is a keypress (or was sent via ./client.exe -k). 
	# Otherwise, it is an ordinary event that was sent by executing a function we haven't implemented yet.
	def parse_to_event(string)
		if string[0] == '_'
			# String is a keypress. First, split modifiers and actual key.
			key, modifiers_num = string.split(":")

			# `modifiers_num` is just a 32bit decimal integer. Each bit encodes whether a modifier is pressed.
			modifiers_num = modifiers_num.to_i
			modifiers = []
			Modifiers.each do |mask|
				mod_was_pressed = (modifiers_num % 2) != 0
				modifiers << mask if mod_was_pressed
				modifiers_num >>= 1
			end
			return modifiers.join("_") + key
		else
			return string
		end
	end

end
