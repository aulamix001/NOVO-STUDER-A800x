import json
import unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
class Presets(unittest.TestCase):
 def test_complete_matrix_and_values(self):
  m=json.loads((ROOT/'config/parameters.v1.json').read_text())
  ps=json.loads((ROOT/'presets/factory.json').read_text())['presets']
  self.assertEqual(len(ps),90)
  self.assertEqual(len({p['name'] for p in ps}),90)
  expected={(eq,speed,row['tape']) for eq in ['NAB','CCIR'] for speed in [7.5,15,30] for row in m['bias_table']['rows']}
  self.assertEqual({(p['eq_standard'],p['speed_ips'],p['tape']) for p in ps},expected)
  for p in ps:
   values=p['parameters']; self.assertEqual(set(values),{x['id'] for x in m['parameters']})
   for spec in m['parameters']:
    if spec['type']=='choice': self.assertIn(values[spec['id']],spec['values'])
    if spec['type']=='float' and spec.get('unit')=='dB': self.assertEqual(values[spec['id']],spec['default'])
   pair=m['speed_pairs_ips'][values['machine_version']]
   self.assertEqual(pair[values['speed_select']=='Fast'],p['speed_ips'])
   row=next(r for r in m['bias_table']['rows'] if r['tape']==p['tape'])
   self.assertEqual(row['overbias_db'][m['bias_table']['speeds_ips'].index(p['speed_ips'])],p['effective_overbias_db'])
 def test_bias_reference(self):
  m=json.loads((ROOT/'config/parameters.v1.json').read_text())
  rows=m['bias_table']['rows']
  self.assertEqual(len(rows),15)
  self.assertEqual(sum(len(r['overbias_db']) for r in rows),60)
  self.assertEqual(rows[3]['overbias_db'],[5,6,3.5,1])
  self.assertEqual(rows[8]['overbias_db'],[5,6.5,3.5,1.5])
if __name__=='__main__': unittest.main()
