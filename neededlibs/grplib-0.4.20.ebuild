
EAPI=5
AUTOTOOLS_AUTO_DEPEND="no"

inherit autotools toolchain-funcs multilib multilib-minimal

DESCRIPTION="grplib"
HOMEPAGE=""
SRC_URI="${P}.tar.gz"

LICENSE="LGPL"
SLOT="0"
KEYWORDS="~alpha amd64 ~arm ~arm64 ~hppa ~ia64 ~m68k ~mips ~ppc ~ppc64 ~s390 ~sh ~sparc x86 ~amd64-fbsd ~sparc-fbsd ~x86-fbsd"
IUSE="asm debug"

RDEPEND=""
DEPEND=""

src_prepare() {
	multilib_copy_sources
}

multilib_src_configure() {
	local myconf=""
	if use asm ; then
#	    if use x86 || use x86-linux ; then
		myconf="${myconf} --enable-asm"
#	    fi
	fi
	if use debug ; then
	    myconf="${myconf} --enable-debug"
	fi
	econf ${myconf} \
	    --prefix="${EPREFIX}/usr" \
	    --libdir="${EPREFIX}/usr/$(get_libdir)" \
	    || die
}

multilib_src_compile() {
		emake
}

multilib_src_install() {
		insinto /usr/$(get_libdir)/pkgconfig
		doins grp.pc
		emake install DESTDIR="${D}" LDCONFIG=:
		gen_usr_ldscript -a grp
}

multilib_src_install_all() {
	dodoc README ChangeLog
}
