#!/bin/bash
file_name="firmware/project_folder/project_folder.runs/impl_1/main_v2.bit"
rpi="pi@nexysddmtd3.local"


scp $file_name $rpi:


ssh -T $rpi << EOF
openFPGALoader -b nexysVideo main_v2.bit 
EOF

# 9e8d94b5f11c2fcffbc97ee88db67a853628c676