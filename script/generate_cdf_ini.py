import pathlib
import numpy as np
import pandas as pd


def main():
    data_dir = pathlib.Path(__file__).parent.parent / \
        'src' / 'testbed'/'distribution'/'data'

    generated_inc = []
    for raw_filename in data_dir.glob('*.txt'):
        cdf = pd.read_csv(
            raw_filename,
            names=['length', 'p'],
            header=None,
            delim_whitespace=True)

        name = raw_filename.stem
        generated_inc.append(name)
        inc_filename = f'{name}.inc'
        with open(data_dir/inc_filename, 'w') as inc_file:
            inc_file.write(f'const auto {name} = CdfDistribution({"{"}\n')
            for _, row in cdf.iterrows():
                inc_file.write(f'    {"{"}{int(row.length)}, {row.p}{"}"},\n')
            inc_file.write('});')

    with open(data_dir/'table.inc', 'w') as inc_file:
        for name in generated_inc:
            inc_file.write(
                f'#include "testbed/distribution/data/{name}.inc"\n')
        inc_file.write('\n')
        inc_file.write(
            'std::map<std::string, CdfDistribution> CdfDistribution::table{\n')
        for name in generated_inc:
            inc_file.write(f'    {"{"}"{name}", {name}{"}"},\n')
        inc_file.write('};')


if __name__ == '__main__':
    main()
