#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib-xcb.h>
#include <X11/Xutil.h>
#include <unistd.h>

				
int main(){
	Display *dpy = XOpenDisplay(NULL);
	xcb_connection_t *connection = XGetXCBConnection(dpy);
	xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
	xcb_window_t window = xcb_generate_id(connection);
	uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        uint32_t     values[2] = {screen->white_pixel,
                                    XCB_EVENT_MASK_EXPOSURE       | XCB_EVENT_MASK_BUTTON_PRESS   |
                                    XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
                                    XCB_EVENT_MASK_KEY_PRESS      | XCB_EVENT_MASK_KEY_RELEASE };
        xcb_create_window (connection,
                           0,                             /* depth               */
                           window,
                           screen->root,                  /* parent window       */
                           0, 0,                          /* x, y                */
                           150, 150,                      /* width, height       */
                           10,                            /* border_width        */
                           XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                           screen->root_visual,           /* visual              */
                           mask, values );                /* masks */
	xcb_map_window (connection, window);
        xcb_flush (connection);
	xcb_generic_event_t *event;
	// receive events. As of now, most are ignored. Will implement mouse events as soon as the rest works.
        while ( (event = xcb_wait_for_event(connection)) ) {
		switch(event->response_type & ~0x80) {
			case XCB_BUTTON_PRESS: {
				xcb_button_press_event_t *button_press = (xcb_button_press_event_t *) event;
				break;
				}
			case XCB_BUTTON_RELEASE: {
				xcb_button_release_event_t *button_release = (xcb_button_release_event_t *) event;
				break;
						 }
			case XCB_KEY_PRESS: {
				xcb_key_press_event_t *press = (xcb_key_press_event_t *) event;
				char str[256 + 1];
				int byte_amount;
				KeySym ks;
				KeyCode kc = 0;
				const char *ksname;

				/* pretty fucking stupid hack, but unfortunately xcb does not provide XLookupString, so we piece together our own XKeyEvent from the keypress event we received.
				 * In case something breaks, I blame 50% of it on xcb for not providing XLookupString, and 50% of it on me for implementing it in the worst possible way.
				 * If someone knows a better way, *please* tell me. At the time of writing this, I'm new to xcb and just spent 2 days sifting through unavailable documentation and wildly typing 
				 * terms into my search engine in order to leverage wisdom from the Blagoblag.
				 */
				XKeyEvent e = {press->response_type, 0, False, dpy, press->event, press->root, press->child, press->time, 0, 0, 0, 0, (unsigned int) press->state, (unsigned int) press->detail, (Bool) press->same_screen};
				byte_amount = XLookupString(&e, str, 256, &ks, NULL);

				if(ks == NoSymbol)
					ksname = "NoSymbol";
				else {
					if (!(ksname = XKeysymToString(ks)))
						ksname = "(no name)";
					kc = XKeysymToKeycode(dpy, ks);
				}
				printf("keysym 0x%lx, %s\n", (unsigned long) ks, ksname);
				break;
						 }
			case XCB_KEY_RELEASE: {
				xcb_key_release_event_t *release = (xcb_key_release_event_t *) event;
				break;
						 }
			case XCB_MOTION_NOTIFY: {
				xcb_motion_notify_event_t *motion = (xcb_motion_notify_event_t *) event;
				break;
						 }
			default:
				/* Unknown event type, ignore it */
				printf ("Unknown event: %PRIu8\n",
					event->response_type);
				break;
		}
		free(event);
	}
	return 0;
}
