#!/bin/sh
# the next line restarts using wish \
exec wish4.2 "$0" "$@"


# load text from file into window

proc loadText {w name} {
#    $w configure -state normal
    set f [open $name]
    while {![eof $f]} {
        gets $f s
        if {[string first "-" $s] == 0 || [string first "=" $s] == 0} {
            $w insert end $s tag_emph
        } elseif {[string first "Testing: " $s] == 0} {
            $w insert end $s tag_emph
        } elseif {[string first "Pass " $s] == 0} {
            $w insert end $s tag_emph
        } elseif {[string first "Error: " $s] == 0} {
            $w insert end $s tag_err
        } elseif {[string first "Warning: " $s] == 0} {
            $w insert end $s tag_warn
        } elseif {[string first "Informational: " $s] == 0} {
            $w insert end $s tag_inf
        } elseif {[string first "Test passed" $s] == 0} {
            $w insert end $s tag_passed
        } elseif {[string first "Test failed" $s] == 0} {
            $w insert end $s tag_failed
        } else {
            $w insert end $s
        }
        $w insert end "\n"
    }
    close $f
#    $w configure -state disabled
}


# search string in text window and mark all positions with tag

proc findText {w name tag} {
    $w tag remove $tag 1.0 end
    set hits 0
    set pos -1
    if {$name != ""} {
        set cur 1.0
        while 1 {
            set cur [$w search -count length -- $name $cur end]
            if {$cur == ""} {
                break
            }
            if {$pos < 0} {
                set pos $cur
            }
            $w tag add $tag $cur "$cur + $length char"
            set cur [$w index "$cur + $length char"]
            incr hits
        }
    }
    if {$pos >= 0} {
        $w see $pos
    }
    return $hits
}

# search string in both text windows

proc findTextCommand {name} {
    global hitsText

    set dh [findText .report.text $name tag_found]
    set hitsText [concat "Hits: " $dh]
    focus .find.entry
    .find.entry delete 0 end
}


# set up text window

proc setupTextWindow {} {
    global findName
    global hitsText
    
    set hitsText "Hits: 0"
 
    frame .find
    button .find.button -text "Find" -command "findTextCommand  \$findName" -width 4 -anchor w
    entry .find.entry -textvariable findName
    label .find.label -textvariable hitsText -width 13
    pack .find.button -side left
    pack .find.label -side right 
    pack .find.entry -fill x -expand yes -padx 5

    frame .report
    text .report.text -height 1 -wrap none -background #fff7e9 -selectbackground lightgray -font -*-Courier-Medium-R-Normal--*-150-*-*-*-*-*-* -xscrollcommand ".report.xscroll set" -yscrollcommand ".report.yscroll set"
    scrollbar .report.xscroll -orient horizontal -command ".report.text xview"
    scrollbar .report.yscroll -orient vertical -command ".report.text yview"
    pack .report.xscroll -side bottom -fill x
    pack .report.yscroll -side right -fill y
    pack .report.text -expand yes -fill both
    
    .report.text tag configure tag_emph -foreground black -font -*-Courier-Bold-R-Normal--*-150-*-*-*-*-*-*
    .report.text tag configure tag_found -background lightgray -underline true
    .report.text tag configure tag_warn -background #ffff8c
    .report.text tag configure tag_err -background #ff8c8c
    .report.text tag configure tag_inf -background #aae6aa
    .report.text tag configure tag_passed -foreground white -background black -font -*-Courier-Bold-R-Normal--*-150-*-*-*-*-*-*
    .report.text tag configure tag_failed -foreground white -background black -font -*-Courier-Bold-R-Normal--*-150-*-*-*-*-*-*

    pack .find -side top -fill x
    pack .report -expand yes -fill both

    bind .find.entry <Return> "findTextCommand  \$findName"
    bind .report.text <Button-3> {if {[.report.text tag nextrange sel 1.0] != ""} {findTextCommand [selection get]}}
    
    focus .find.entry
}


# main window
catch {eval destroy [winfo child .]} errMsg
if {$errMsg > 0} {
   exit 1
}

# initialization
set temp "/tmp"
if [info exists env(TEMP)] {
   set temp $env(TEMP)
} elseif [info exists env(TMP)] {
   set temp $env(TMP)
}
set checkReport  [file join $temp dcmpschk.report]
set checkApp "dcmpschk"

# set up window
wm iconname . "DICOM Checker"
if {$tcl_platform(os) == "SunOS"} {
   wm geometry . 956x903
} else {
   wm geometry . [expr int([winfo screenwidth .] * 0.995)]x[expr int([winfo screenheight .] * 0.938)]
}
wm geometry . +0+0
wm minsize . 400 165

setupTextWindow

# set up bindings
bind all <Control-c> {destroy .}

# start check process
if {$argc > 0} {
  set fileName [lindex $argv 0]
  wm title . "Please wait ..."
  catch {exec $checkApp -v $fileName >& $checkReport}
  loadText .report.text $checkReport
  update
  wm title . [format "DICOM PState Checker  -  %s" $fileName]
} else {
  wm title . [format "DICOM PState Checker  -  <no filename specified>"]
}
