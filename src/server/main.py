import json
from warnings import warn

from models.client import Client


def main():
    config = _get_config()
    # port = _get_port(config['port_file'])
    c = Client()


def _get_config() -> dict:
    try:
        with open('config.json', 'r') as f:
            config = json.load(f)
            return config
    except FileNotFoundError:
        warn('config.json not found.')
    except json.JSONDecodeError:
        warn('Invalid JSON in config.json.')
    exit(1)


def _get_port(path: str) -> int:
    try:
        with open(path, 'r') as f:
            port = int(f.read().strip())
            return port
    except FileNotFoundError:
        warn(f'"{path}" not found. Using default port 1357.')
    except ValueError:
        warn(f'Invalid port number in "{path}". Using default port 1357.')
    return 1357


if __name__ == "__main__":
    main()
