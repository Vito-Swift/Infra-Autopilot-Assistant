from telnetlib import Telnet
import pynmea2


with Telnet('192.168.57.1', 6001) as tn:
    while True: 
        try: 
            line = tn.read_until(b"\r\n").decode()
            msg = pynmea2.parse(line)
            if msg.sentence_type == "GGA": 
                print(f'position lng: {msg.lon}  lat: {msg.lat}')
            elif msg.sentence_type == "GSV":
                # satellite data, no use
                pass 
            elif msg.sentence_type == "GSA":
                # precision data, no use
                pass
            elif msg.sentence_type == "GLL":
                # position
                pass
            elif msg.sentence_type == "RMC":
                # possibily useful
                pass
            elif msg.sentence_type == "VTG":
                # track
                pass
            else : 
                print(msg.sentence_type)
            # print(repr(msg))
        except pynmea2.ParseError as e:
            print('Parse error: {}'.format(e))
            continue
