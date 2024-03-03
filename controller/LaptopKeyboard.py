from pynput.keyboard import Key, Listener, KeyCode

key_state = {}

# Function to be called when a key is pressed
def on_press(key):
    key_state[key] = True  # Update key state to pressed

# Function to be called when a key is released
def on_release(key):
    key_state[key] = False  # Update key state to released
    
def get_key_state(key):
    if type(key) == str:
        key = KeyCode.from_char(key)
    if (key in key_state and key_state[key]):
        return True
    else:
        return False