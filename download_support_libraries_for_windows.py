#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import threading
import requests
import subprocess
import bs4
import zipfile
import argparse


def download(url: str):
    print(f"downloading {url}")
    res = requests.get(url, timeout=120)
    if res.ok:
        file_name = url.split("/")[-1]

        print(f"save {url}")
        with open(f"{file_name}", "wb") as f:
            f.write(res.content)

        print(f"unzip {url}")
        with zipfile.ZipFile(file_name) as zip:
            zip.extractall()

        print(f"remove {url}")
        os.remove(file_name)


def main(rt: str, cv: str, arch: str):
    # get latest version
    ver = subprocess.run(["git", "describe", "--abbrev=0",
                          "--tags"], capture_output=True).stdout.decode('ascii').removeprefix("DCMTK-").removesuffix('\n')

    rt = rt.upper()
    print(
        f"downloading dcmtk support libraies for windows:{ver} {rt.upper()} {cv} {arch}bit")
    ver_no_dot = ver.replace('.', '')

    # get page
    url = f"https://dicom.offis.de/download/dcmtk/dcmtk{ver_no_dot}/support/"
    resp = requests.get(url)
    soup = bs4.BeautifulSoup(resp.text, "html.parser")

    # parse page
    urls = []
    for table in soup.find_all("table"):
        row_size = len(table)
        if row_size < 20:
            continue
        for row in table.find_all("tr"):
            for column in row.find_all("td"):
                chapter = column.find("a")
                if chapter:
                    link = chapter.attrs["href"]
                    if "dcmtk-{}".format(ver) in link and rt in link and cv in link and f"win{arch}" in link:
                        urls.append(url+link)
    workers = []
    for url in urls:
        worker = threading.Thread(target=download, args=(url,))
        workers.append(worker)
        worker.start()
    for worker in workers:
        worker.join()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="download support libraries for windows")
    parser.add_argument(
        "-r", "--runtime", help="windows runtime, md(default) for miultithread dynamic link, mt for multithread static link", default="md", choices=["md", "mt"])
    parser.add_argument(
        "-c", "--conv", help="character conversion, iconv(default) for libiconv(https://www.gnu.org/software/libiconv/), icu for icu(http://site.icu-project.org/)", default="iconv", choices=["iconv", "icu"])
    parser.add_argument(
        "-a", "--arch", help="arch, 32 for 32 bit, 64(default) for 64 bit", default="64", choices=["32", "64"])
    args = parser.parse_args()
    main(args.runtime, args.conv, args.arch)
