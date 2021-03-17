import glob
import json
import os
import nbformat
from nbconvert.preprocessors import ExecutePreprocessor


ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def run(infile, outfile, timeout=1200):
    print("Executing %s..." % infile)

    # Open the notebook
    with open(infile, "r") as f:
        nb = nbformat.read(f, as_version=4)

    # Pre-process
    for cell in nb.get("cells", []):
        if "hide_input" in cell.get("metadata", {}).get("tags", []):
            cell["source"] = "#hide_input\n" + cell["source"]

    # Execute the notebook
    if nb["metadata"].get("nbsphinx_execute", True):
        ep = ExecutePreprocessor(
            timeout=timeout, kernel_name="python3", store_widget_state=True
        )
        ep.preprocess(
            nb,
            {"metadata": {"path": os.path.dirname(os.path.abspath(infile))}},
        )

    # Post-process
    for cell in nb.get("cells", []):
        if cell.get("metadata", {}).get("replace_input", None) is not None:
            cell["source"] = cell.get("metadata", {}).get(
                "replace_input", None
            )

    # Write it back
    with open(outfile, "w", encoding="utf-8") as f:
        nbformat.write(nb, f)


if __name__ == "__main__":
    # Run the notebookss
    files = glob.glob(
        os.path.join(ROOT, "docs", "notebooks", "*_template.ipynb")
    )
    for infile in files:
        run(infile, infile.replace("_template", ""))
