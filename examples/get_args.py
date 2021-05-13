import argparse


def get_args():
    parser = argparse.ArgumentParser(prog="makeplot", add_help=True)
    parser.add_argument(
        "ext",
        nargs="?",
        default="pdf",
        choices=["pdf", "png"],
        help="Output figure extension",
    )
    return parser.parse_args()
