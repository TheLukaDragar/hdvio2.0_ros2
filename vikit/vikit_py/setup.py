from setuptools import setup

package_name = 'vikit_py'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    package_dir={'': 'src'},
    install_requires=['setuptools', 'pyyaml', 'numpy', 'psutil'],
    zip_safe=True,
    maintainer='Christian Forster',
    maintainer_email='forster@ifi.uzh.ch',
    description='The vikit_py package',
    license='BSD',
)