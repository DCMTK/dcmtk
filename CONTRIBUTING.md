# Contributing to the DCMTK

First of all, thanks for taking the time to contribute!

All types of contributions are welcome. See the [Table of contents](#table-of-contents) for different ways to help and details about how we handle them. Please make sure that you read the relevant section before making your contribution. This will make it much easier for us maintainers and simplify the work for everyone involved. We look forward to your contributions.


## Table of contents

- [I have a question](#i-have-a-question)
- [I want to contribute](#i-want-to-contribute)
  - [Reporting bugs](#reporting-bugs)
  - [Suggesting enhancements](#suggesting-enhancements)


## I have a question

> If you want to ask a question, we assume that you have read the available [documentation](https://support.dcmtk.org/docs/).

Before you ask a question, you should search for existing [issues](https://support.dcmtk.org/redmine/projects/dcmtk/issues) and [forum postings](https://forum.dcmtk.org/) that might help you.

If you then still feel the need to ask a question and need clarification, we recommend the following:

- [Register](https://forum.dcmtk.org/viewtopic.php?t=5317) for the public [DCMTK forum](https://forum.dcmtk.org/) if you have not already done so.
- Post your question in the appropriate forum, e.g. [DCMTK - General](https://forum.dcmtk.org/viewforum.php?f=1) or [DCMTK - Installation](https://forum.dcmtk.org/viewforum.php?f=3).
- Provide as much context as possible about the problem you are facing.
- Include information on the DCMTK version and platform (if relevant).

One of our team members or another forum user will then hopefully take care of your request (as time permits).


## I want to contribute

> ### Legal notice
> When contributing to the DCMTK, you must agree that you have authored 100% of the content, that you have the necessary rights to the content and that the content you contribute may be provided under the [DCMTK license](https://support.dcmtk.org/docs/file_copyright.html), i.e. it needs to be compatible with the [BSD License](https://opensource.org/license/bsd-3-clause).

### Reporting bugs

#### Before submitting a bug report

A good bug report shouldn't leave others needing to chase you up for more information. Therefore, we ask you to investigate carefully, collect information and describe the issue in detail in your report. Please complete the following steps in advance to help us fixing any potential bug as fast as possible.

- Make sure that you are using the [latest version](https://git.dcmtk.org/?p=dcmtk.git;a=summary) of the DCMTK.
- Determine if your bug is really a bug and not an error on your side, e.g. using incompatible environment components/versions. Make sure that you have read the [documentation](https://support.dcmtk.org/docs/). If you are looking for support, you might want to check [this section](#i-have-a-question).
- To see if other users have experienced (and potentially already solved) the same issue you are having, check if there is not already a bug report existing for your problem in the [issue tracker](https://support.dcmtk.org/redmine/projects/dcmtk/issues).
- Collect information about the bug:
  - OS, platform and version (Windows, Linux, macOS, x86, x64, etc.)
  - Name and version of the compiler, support libraries, etc., depending on what seems relevant.
  - Possibly your input and the output, log messages in debug/trace mode, stack trace and the like.
  - Can you reliably reproduce the issue? And, can you also reproduce it with older versions?

#### How do I submit a good bug report?

> You must never report security related issues, vulnerabilities or bugs including sensitive information to the [DCMTK forum](https://forum.dcmtk.org/), or elsewhere in public. Instead sensitive bugs must be sent by email to bugs@dcmtk.org.

We use our own [issue tracker](https://support.dcmtk.org/redmine/projects/dcmtk/issues) for bugs and other issues. If you run into a problem with the DCMTK, please proceed as follows:

- If you are sure that you've found a bug in the DCMTK, send an email to [bugs@dcmtk.org](mailto:bugs@dcmtk.org). Otherwise, post your problem to the [DCMTK forum](https://forum.dcmtk.org/).
- Explain the behavior you would expect and the actual behavior.
- Please provide as much context as possible and describe the *reproduction steps* that someone else can follow to recreate the issue on their own. This usually includes your code. For good bug reports, you should isolate the problem and create a reduced test case, e.g. as [a minimal, reproducible example](https://stackoverflow.com/help/minimal-reproducible-example).
- Provide the information you collected in the previous section.
- If you have already analyzed the problem and found a fix, we would be happy to get it either by [email](mailto:bugs@dcmtk.org) (with a patch file attached) or as a [GitHub pull request](https://github.com/DCMTK/dcmtk/pulls).

Once it is filed:

- The DCMTK team will check if it is really a bug and this issue has not been fixed yet, or if you were just having a question (see the [above section](#i-have-a-question)).
- A team member will try to reproduce the issue with your provided steps. If there are no reproduction steps or there is no obvious way to reproduce the issue, the team will probably ask you for those steps.
- If the team is able to reproduce the issue, it will either be fixed directly or this problem will be added as a new issue to the [issue tracker](https://support.dcmtk.org/redmine/projects/dcmtk/issues) and processed later.

### Suggesting enhancements

This section guides you through submitting an enhancement suggestion for the DCMTK, including completely new features and minor improvements to existing functionality. Following these guidelines will help us to understand your suggestion and find related suggestions.

*TODO*


## Credits

This guide was inspired by [contributing-gen](https://github.com/bttger/contributing-gen).
