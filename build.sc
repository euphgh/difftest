import mill._
import scalalib._
import mill.scalalib.scalafmt.ScalafmtModule

/** the base param, can be overrided by parent project if needed */
trait ChiselModule extends ScalaModule with ScalafmtModule {
  override def scalaVersion = "2.13.10"

  override def ivyDeps = Agg(
    ivy"edu.berkeley.cs::chisel3:3.6.0"
  )

  override def scalacPluginIvyDeps = Agg(
    ivy"edu.berkeley.cs:::chisel3-plugin:3.6.0"
  )

  override def scalacOptions = Seq(
    "-Ymacro-annotations",
    "-Xfatal-warnings",
    "-feature",
    "-deprecation",
    "-language:reflectiveCalls"
  )
}

trait DifftestTrait extends ChiselModule

object difftest extends RootModule with DifftestTrait {
  object test extends ScalaTests with TestModule.ScalaTest {
    override def ivyDeps = super.ivyDeps() ++ Agg(
      ivy"edu.berkeley.cs::chiseltest:0.6.2"
    )
  }
}
