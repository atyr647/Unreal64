import sys, numpy as np
from PIL import Image, ImageDraw
S=sys.argv[1]; out=sys.argv[2]   # S/cmp holds linux_N.ppm (render_host) and ares_N.png (ares screenshots)
frames=[15,74,90,183,245,336,420,470,520,660]
rows=[]; stats=[]
for n in frames:
    lin=Image.open(f"{S}/cmp/linux_{n}.ppm").convert("RGB")
    a=Image.open(f"{S}/cmp/ares_{n}.png").convert("RGB")
    crop=a.crop((0,4,1022,768)).resize((320,240),Image.BILINEAR)
    xs=ys=[0]
    L=np.asarray(lin).astype(int); A=np.asarray(crop).astype(int)
    d=np.abs(L-A).sum(2)
    mae=np.abs(L-A).mean(); close=(d<=60).mean()*100
    diff=Image.fromarray(np.clip(d,0,255).astype(np.uint8)).convert("RGB")
    stats.append((n,mae,close))
    row=Image.new("RGB",(960,240+14))
    row.paste(lin,(0,14)); row.paste(crop,(320,14)); row.paste(diff,(640,14))
    dr=ImageDraw.Draw(row); dr.text((2,1),f"frame {n}  LINUX",fill=(255,255,255)); dr.text((322,1),"ARES (N64)",fill=(255,255,255)); dr.text((642,1),f"|diff|  MAE={mae:.1f}  {close:.0f}% px within tol",fill=(255,255,0))
    rows.append(row)
    print(n,f"MAE={mae:.2f} close={close:.1f}% ")
sheet=Image.new("RGB",(960,254*len(rows)))
for i,r in enumerate(rows): sheet.paste(r,(0,254*i))
sheet.save(out)
# two sheets of 5 for viewing
for k in range(2):
    s=Image.new("RGB",(960,254*5))
    for i,r in enumerate(rows[k*5:k*5+5]): s.paste(r,(0,254*i))
    s.save(out.replace(".png",f"_{k}.png"))
