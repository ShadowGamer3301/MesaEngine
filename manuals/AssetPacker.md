# AssetPacker
Asset packer is a tool designed to pack assets into
archives.

## Usage
AssetPacker in its current version looks for four files:
- textures.pcdef
- materials.pcdef
- models.pcdef
- shaders_dx.pcdef

These files contain defintions for all archives that will be generated.
If none of those files exist in your project directory simply create them
and fill them with content.

## Defining pcdef file
PCDEF (package definition) file are read line by line. 
This means that you can only define one asset per line.
Lines starting with $ sign are treated as beggining of new package.
Current version of AssetPacker requires to have all files belonging 
to one package defined next to each other like in the example below:

``` 
$MyPack1.mtp
File1.png
File2.png
Directory1/File3.png

$MyPack2.mtp
File4.png
File5.png
```

If you define you packages like this they will break down!!! DO NOT DO THIS!!!
``` 
$MyPack1.mtp
File1.png

$MyPack2.mtp
File4.png
File5.png

$MyPack1.mtp
File2.png
Directory1/File3.png
```

## Lookup table
After AssetPacker is done packing it will generate file called lookup.csv where
information about archives is stored. This file will be used later to load
individual assets from the archives.