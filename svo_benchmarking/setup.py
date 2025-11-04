from setuptools import setup

package_name = 'svo_ceres_benchmarking'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    package_dir={'': 'src'},
    install_requires=['setuptools', 'pyyaml', 'numpy'],
    zip_safe=True,
    maintainer='Zichao Zhang',
    maintainer_email='zzhang@ifi.uzh.ch',
    description='svo_benchmarking utils',
    license='BSD',
)
