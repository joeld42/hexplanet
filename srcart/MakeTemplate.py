import os, sys, string
import math
from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

# This script is a hacky mess.
# That's ok because it's disposable :)

# Source art in priority order (higher will be
# atop lower)
HEXSRC = [
    "hexsrc_ocean.png",
    "hexsrc_desert.png",
    "hexsrc_grassland.png",
    "hexsrc_forest.png",
    "hexsrc_mountain.png"
    ]

# Make a tile.. see template
#     |  
#  B  |  C
#    / \
#   / A \
#  /     \
def MakeTile( srcImgs, a, b, c ):
    
    imgA = srcImgs[a]
    imgB = srcImgs[b]
    imgC = srcImgs[c]

    # Get the clips from the src images
    clipA = imgA.crop( (0,0,256,256 ) )
    clipB = imgB.crop( (256,0,512,256 ) )
    clipC = imgC.crop( (512,0,768,256 ) )
    
    # Prioritize by index
    clips = [ (a, clipA), (b, clipB), (c, clipC) ]
    clips.sort()    
    
    imgDest = clips[0][1]
    imgDest.paste( clips[1][1], None, clips[1][1] )
    imgDest.paste( clips[2][1], None, clips[2][1] )

    return imgDest
    

# main
if __name__=='__main__':

    # Make a new, transparent image
    img = Image.new( "RGB", (256,256), "#ffc600" )

    # draw guide lines on it
    cx = 128
    cy = 128
    crad = 120
    pnts = []
    pnts2 = []

    # Make an equalateral triangle
    t = 0.0    
    while (t < math.pi * 2.0):
        p = ( ( (math.sin(t)*0.45 + 0.5, math.cos(t)*0.45 + 0.5 ) ) )
        p2 = ( ( (math.sin(t + math.pi/3)*0.45 + 0.5, math.cos(t+ math.pi/3)*0.45 + 0.5 ) ) )

        print p
        pnts.append( (p[0] * img.size[0], p[1] * img.size[1]))
        pnts2.append( (p2[0] * img.size[0], p2[1] * img.size[1]))
               
        t += math.pi * (2.0/3.0)

    draw = ImageDraw.Draw( img )
    
    # draw the triangle
    draw.polygon( pnts, fill="#ffc600" )

    # the "dual" outline
    draw.line( (pnts[0][0], pnts[0][1], pnts[1][0], pnts[1][1]), fill="#eab600", width=5 )
    draw.line( (pnts[1][0], pnts[1][1], pnts[2][0], pnts[2][1]), fill="#eab600", width=5 )
    draw.line( (pnts[0][0], pnts[0][1], pnts[2][0], pnts[2][1]), fill="#eab600", width=5 )

    # the "hex" border
    for p in pnts2:
        draw.line( (cx, cy) + p, fill="#ffffff", width=8 )


    # save the template
    img.save( "template.png" )

    # load the source images
    srcImgs = []
    for s in HEXSRC:
        img = Image.open( s )
        srcImgs.append( img )
        
    # Make a test tile
    #img2 = MakeTile( srcImgs, 4, 0, 2 )
    #img2 = img2.convert( "RGB" )
    #img2.save( "testtile.png")

    # For debugging
    font = ImageFont.truetype( "c:\\windows\\fonts\\ARIBLK.TTF", 48 )

    # Make a big image
    # 12 x 12 tiles = 144 tiles capacity
    # 1024 map = 85 pxls per tile
    imgTileSet = Image.new( "RGB", (1024,1024), "#ffffff" )
    for a in range( 0, 5 ):
        for b in range( 0, 5 ):
            for c in range( 0, 5 ):
                print (a,b,c),
                # Make this tile
                imgTile = MakeTile( srcImgs, a, b, c )

                # save tiles separately for debugging
                #imgTile.save( ("debug/tile%d_%d_%d.png" % (a,b,c) ) )

                doOutline = 0
                if (doOutline):
                    draw = ImageDraw.Draw( imgTile )    
                    # the "hex" border
                    for p in pnts2:
                        draw.line( (cx, cy) + p, fill="#ffffff", width=12 )

                    # DBG: Tile index
                    #index = (a*25)+(b*5)+c
                    #draw.text( (cx-16, cy-16), str(index), fill="#000000", font=font )
                    #draw.text( (cx-20, cy-20), str(index), fill="#ffffff", font=font )

                # add it to the image
                index = (a*25)+(b*5)+c
                tile_y = index / 12
                tile_x = index % 12
                print "index", index, (tile_x, tile_y),
                imgTile = imgTile.resize( (85, 85), Image.ANTIALIAS )
                #imgTile.save( ("debug/tile%d_%d_%d.png" % (a,b,c) ) )
                
                box = (tile_x * 85, tile_y * 85, (tile_x+1)*85, (tile_y+1)*85)
                print box
                imgTileSet.paste( imgTile, box )

    imgTileSet.save( "tileset.png" )
                

               
