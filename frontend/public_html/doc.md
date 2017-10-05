# Pocoweb

Pocoweb is a web based postcorrection system for OCRed historical documents.
It is loosely based on [PoCoTo](https://github.com/cisocrgroup/PoCoTo).

Users can upload their OCRed documents and correct them in the web frontend.
The corrected documents can then be downloaded and further processed.
Pocoweb understands various OCR formats that can be used.
Any corrections are written back into the original format of the documents.
This makes it possible to integrate the manual postcorrection of OCRed documents
into other digitalisation workflows.

As PoCoTo, Pocoweb is connected with a language
[profiler](https://github.com/cisocrgroup/Profiler)
that enables semi-automatic correction of historical documents.
The profiler is able to differentiate between historical spelling variation
and real OCR errors in the documents and generates correction suggestions
for suspicious words.

- - -
## Table of contents

* [Users](#users)
* [User management](#user-management)

- - -
<a id='users'></a>
## Users

In order to use Pocoweb, each user has to login in the
[login page](/login.php) using her username and password.

There are two kinds of user accounts in Pocoweb, that have
different permissions in the system:

1. Administrator accounts
    * Create new administrator and normal users accounts
    * Delete user accounts
    * Upload new documents and create new [projects](#projects)
    * Split projects into [packages](#packages)
    * Assign packages to different user accounts
    * Profiling projects.
    * Post correct projects and packages.
    * Delete documents and packages.
    * Download corrected projects

2. Normal user accounts
    * Post correct packages that where assigned to them
    * Profiling projects.
    * Reassign packages back to their original owner.

- - -
<a id='user-management'></a>
## User management

In order to remove an user account click on the according

<span class="glyphicon glyphicon-remove">foobar</span>

button in the user list.
