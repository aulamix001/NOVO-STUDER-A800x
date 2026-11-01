#!/usr/bin/env python3
"""Calibration presets from the supplied specification, not verified manuals."""
import json
from pathlib import Path
root=Path(__file__).resolve().parents[1]
m=json.loads((root/'config/parameters.v1.json').read_text())
base={p['id']:p['default'] for p in m['parameters']}
out=root/'presets/factory'
out.mkdir(parents=True, exist_ok=True)
manifest=[]
for eq in ['NAB','CCIR']:
 for speed in [7.5,15,30]:
  for tape_index,row in enumerate(m['bias_table']['rows']):
   values=dict(base)
   values.update(eq_standard=eq,machine_version={7.5:'15/7.5',15:'15/7.5',30:'30/15'}[speed],speed_select='Slow' if speed==7.5 else 'Fast',tape_type=row['tape'])
   name=f"Alinhamento {eq} {speed:g} ips {row['tape']}"
   entry=dict(schema_version=1,name=name,category='Alinhamento',source='user_specification_table_2_6_unverified',speed_ips=speed,eq_standard=eq,tape=row['tape'],effective_overbias_db=row['overbias_db'][m['bias_table']['speeds_ips'].index(speed)],parameters=values)
   filename=f'{eq}_{speed:g}_{tape_index+1:02d}.json'
   (out/filename).write_text(json.dumps(entry,ensure_ascii=False,indent=2)+'\n')
   manifest.append(entry)
(root/'presets/factory.json').write_text(json.dumps(dict(schema_version=1,presets=manifest),ensure_ascii=False,indent=2)+'\n')
print(f'{len(manifest)} calibration presets generated')
