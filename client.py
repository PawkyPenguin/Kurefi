#! /usr/bin/env python

import xcffib
from xcffib.xinput import *
from xcffib.xproto import *
import xcffib.render
import socket


def find_format(screen):
    for d in screen.depths:
        if d.depth == depth:
            for v in d.visuals:
                if v.visual == visual:
                    return v.format

    raise Exception("Failed to find an appropriate Render pictformat!")

def startup():
    white = setup.roots[0].white_pixel

    x_connection.core.CreateWindow(depth, window, root, 0, 0, 640, 480, 0, WindowClass.InputOutput, visual, CW.BackPixel | CW.EventMask, [white, EventMask.ButtonPress | EventMask.Exposure | EventMask.PointerMotion | EventMask.KeyPress])
    cookie = x_connection.render.QueryPictFormats()
    reply = cookie.reply()
    pict_format = find_format(reply.screens[0])

    name = 'Wooo, Imma window :D'
    x_connection.core.ChangeProperty(PropMode.Replace, window, Atom.WM_NAME, Atom.STRING, 8, len(name), name)
    x_connection.render.CreatePicture(pid, window, pict_format, 0, [])
    x_connection.core.MapWindow(window)
    x_connection.flush()


def paint():
    x_connection.core.ClearArea(False, window, 0, 0, 0, 0)

    for x in range(0, 6):
        for y in range(0, 4):
            rectangle = ((x + 1) * 24 + x * 64, (y + 1) * 24 + y * 64, 64, 64)
            new_rectangle = RECTANGLE.synthetic(x=int(rectangle[0]), y=int(rectangle[1]), width=int(rectangle[2]), height=int(rectangle[3]))
            color = (x * 65535 // 7, y * 65535 // 5, (x * y) * 65535 // 35, 65535)
            new_color = xcffib.render.COLOR.synthetic(red=int(color[0]), blue=int(color[1]), green=int(color[2]), alpha=int(color[3]))
            x_connection.render.FillRectangles(xcffib.render.PictOp.Src, pid, new_color, 1, [new_rectangle])

    x_connection.flush()
    
def run():
    startup()
    print('click window to exit')
    while True:
        try:
            event = x_connection.wait_for_event()
        except xcffib.ProtocolException as error:
            print("Protocol error %s received!" % error.__class__.__name__)
            break
        except Exception as error:
            print("Unexpected error received: %s" % error.message)
            break

        if isinstance(event, ExposeEvent):
            paint()
        elif isinstance(event, KeyPressEvent):
            print("Key %d pressed" % event.detail)
        elif isinstance(event, ButtonPressEvent):
            print("Button %d down" % event.detail)
        elif event.response_type == 6:
            print("Movement detected: %d" % event.detail)
            print("Movement was at (%d, %d)" % (event.root_x, event.root_y))
        else:
            print("Event: %d" % event.response_type)


    x_connection.disconnect()




server_socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
server_socket.connect("asdf")

x_connection = xcffib.connect()
x_connection.render = x_connection(xcffib.render.key)
setup = x_connection.get_setup()
root = setup.roots[0].root
depth = setup.roots[0].root_depth
visual = setup.roots[0].root_visual

window = x_connection.generate_id()
pid = x_connection.generate_id()

run()



#server_socket.send("Hallo ich bi dr nils".encode())
