import glob
files = glob.glob ('*')
with open ('VPCINCLUDE.txt', 'w') as in_files:
    for eachfile in files: in_files.write( '			$File	"' + 'build2003\\' + eachfile + '"'+ '\n')