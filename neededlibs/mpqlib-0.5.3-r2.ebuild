
EAPI=5
AUTOTOOLS_AUTO_DEPEND="no"

inherit autotools toolchain-funcs multilib multilib-minimal

DESCRIPTION="mpqlib"
HOMEPAGE=""
SRC_URI="${P}.tar.gz"

LICENSE="LGPL"
SLOT="0"
KEYWORDS="alpha amd64 arm ~arm64 hppa ia64 ~m68k ~mips ppc ppc64 ~s390 ~sh sparc x86 ~amd64-fbsd ~sparc-fbsd ~x86-fbsd"
IUSE="debug"

RDEPEND=""
DEPEND=""

src_prepare() {
	multilib_copy_sources
}

echoit() { echo "$@"; "$@"; }

multilib_src_configure() {
	local myconf=""
	if use debug ; then
	    myconf="${myconf} --enable-debug"
	fi
	echoit "${S}"/configure ${myconf} \
		--prefix="${EPREFIX}/usr" \
		--libdir="${EPREFIX}/usr/$(get_libdir)" \
		|| die
}

multilib_src_compile() {
		emake
}

multilib_src_install() {
		insinto /usr/$(get_libdir)/pkgconfig
		doins mpq.pc
		emake install DESTDIR="${D}" LDCONFIG=:
		gen_usr_ldscript -a mpq
}

multilib_src_install_all() {
	dodoc README ChangeLog
}
