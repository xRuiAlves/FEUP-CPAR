# Installation

In order to run the python script, please install the packages specified in the `requirements.txt`.

To do so, one can use `pip install -r requirements.txt`.

## Virtual Environments

In order to better manage package versions independently, the use of [virtual](https://docs.python.org/3/library/venv.html) [environments](https://docs.python-guide.org/dev/virtualenvs/) is recommended.

Example usage (requires python > 3.3):

```bash
# Create the virtual environment with name 'venv' in the local folder
python -m venv venv
source venv/bin/activate
# Now you are in the virtual environment, any packages installed using `pip` will only be installed here and not system-wide

# In order to exit the virtual environment, just type
deactivate
```

# Running

In order to run the code, after installing the required packages, just:

```bash
./matrixprod.py
```
