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
    parser.add_argument("-q", "--quiet", action="store_true", help="No termnial output")
    parser.add_argument("-rr", "--rerun", action="store_true", help="re-run")
    return parser.parse_args()
