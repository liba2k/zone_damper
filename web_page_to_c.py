#!/bin/env python
import os
import gzip

site_files = ['index.html', 'damper.png', 'reconnecting_websocket.js']

with open(os.path.join('include','web_assets.h'), 'w') as f_out:
    for file_name in site_files:
            print(file_name)
            gz = gzip.compress(open(os.path.join('web',file_name), 'rb').read())
            fn = file_name.replace('.', '_').replace('-', '_')
            f_out.write('const char PROGMEM {}[{}] = {{'.format(fn, len(gz)))
            for b in gz:
                f_out.write('{}, '.format(hex(b)))
            f_out.write('};\n')

