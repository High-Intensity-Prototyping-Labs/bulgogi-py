from bulgogi import Core
import argparse

def cli():
    parser = argparse.ArgumentParser(prog='bul', description='Bulgogi YAML Preview')

    parser.add_argument('files', nargs='+', type=str, metavar='files', help = 'bul file1.yml file2.yml ...')

    args = parser.parse_args()

    if len(args.files) != 0:
        for file in args.files:
            core = Core(from_file=file)
            multifile = len(args.files) > 1

            if multifile:
                print('# ' + file)
            for target in core.targets():
                print(target.name + ' [' + str(target.id) + ']:')
                for dep in target.deps:
                    print('  ' + dep.name + ' [' + str(dep.id) + ']')
                print()

    else:
        parser.print_help()

if __name__ == '__main__':
    cli()
