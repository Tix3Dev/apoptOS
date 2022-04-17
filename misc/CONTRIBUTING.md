# Contributing to apoptOS

When contributing to apoptOS, make sure that the changes you wish to make are in line with the project direction. You should also avoid creating a whole new feature, like a driver. If you are not sure about this, use GitHub Discussions, so we can talk about your ideas.

For your first pull request, start with something small to get familiar with the project and its development processes.

Pull requests may only be accepted, if the follwing guidelines were respected.

## Coding style/formatting

Try to adapt as much as possible to the coding style used in this project. Small anomalies might be updated by maintainers.
For code formatting we make use of a syntax formatting tool [Artistic Style](http://astyle.sourceforge.net/), which you are expected to run before commiting via the command `make format`.

## Comments

It should be clear that you have to document your code with short and consise comments. If this isn't the case, please refer to this [article](https://stackoverflow.blog/2021/12/23/best-practices-for-writing-code-comments/).

## Copyright headers

Your contributions are greatly appreciated, however you must understand that a `[fix] typo` commit cannot be credited in the copyright header.


```
/*
	This file is part of a modern x86_64 UNIX-like microkernel which is called apoptOS
	Everything is openly developed on GitHub: https://github.com/Tix3Dev/apoptOS

	Copyright (C) 2022  Yves Vollmeier <https://github.com/Tix3Dev>
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
	
	Contributor(s): foo
*/
```

- If you have made a *significant* contribution place your name (`Prename Name <https://github.com/Username/>`) next to the `Contributer(s)` field where it says `foo`. If this fieled doesn't exist, please create it.
Multiple contributers should be separated by commas and a newline may be inserted where the list gets too long (if you have to scroll sideways to see your name).
If a newline has been inserted please indent it with 1 tab of the length of 4 spaces

- What does and doesn't qualify for you to place your name in the `Contributer(s)` field:
  - Does: Fixing a security bug, adding some new notable feature (emphasis on notable), or a nasty bug (something that has a very negative impact on the project such as a corruption of xyz due to faulty code)

  - Doesn't: Fixing a typo, correcting a constant, adding variables, refactoring code, adding minor features (a minor feature would be something like a utility function or a wrapper function)

## Commiting

Now that you have followed all of the rules mentioned earlier, you are ready to commit. But it is important to know how.

First of all, split your work into logically seperated commits.

Then describe the commit as short and consise as possible in the commit message. They have to follow the following structure:

```[new] short and consise description``` for new features

```[fix] short and consise description``` for stuff that you had to change in order to make something work

```[refactor] short and consise description``` for stuff that you changed in order to make it better

```[doc] short and consise description``` for changes in the documentation

## Communication

In case you have any questions or desires about/for the project, please use GitHub Discussions to talk to us.
