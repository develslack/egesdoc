// app.js
// =========================================================================
// CONFIGURACIÓN DE ENTORNO (DEV vs PROD)
// =========================================================================
const isDev = (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1');
window.API_BASE_URL = isDev ? 'http://localhost:4000' : '/api';

// Ruta dinámica para las vistas HTML
window.VIEWS_PATH = isDev ? '/views' : '';

console.log("🌐 Entorno detectado. URL Base de la API configurada como:", window.API_BASE_URL);

// =========================================================================
// INDICADOR VISUAL DE ENTORNO (FLOTANTE)
// =========================================================================

const envBadge = document.createElement("div");

// Configurar el texto y el ícono según el entorno
envBadge.innerHTML = isDev
    ? '<span class="glyphicon glyphicon-wrench"></span> ENTORNO DESARROLLO'
    : '<span class="glyphicon glyphicon-globe"></span> ENTORNO PRODUCCIÓN';

// Aplicar estilos flotantes (Abajo a la derecha)
envBadge.style.position = 'fixed';
envBadge.style.bottom = '15px';
envBadge.style.right = '15px';
envBadge.style.padding = '8px 15px';
envBadge.style.borderRadius = '20px';
envBadge.style.color = '#fff';
envBadge.style.backgroundColor = isDev ? '#d35400' : '#27ae60'; // Naranja para Dev, Verde para Prod
envBadge.style.zIndex = '9999';
envBadge.style.fontSize = '12px';
envBadge.style.fontWeight = 'bold';
envBadge.style.boxShadow = '0 4px 6px rgba(0,0,0,0.3)';
envBadge.style.pointerEvents = 'none';

// Inyectar en el documento
document.body.appendChild(envBadge);

// =========================================================================

document.addEventListener("DOMContentLoaded", () => {
  console.log("Panel de Control...");

  // Verificar si existe un usuario en sesión
  const user = JSON.parse(localStorage.getItem("user"));

  if (!user) {
    console.warn("⚠️ No hay sesión activa, redirigiendo al inicio de sesión...");
    alert("⚠️ No hay sesión activa, redirigiendo al inicio de sesión...");
    setTimeout(function() { window.location.href = "/"; }, 3000);
    return;
  } else if (user.token && user.token.length == 128) {
    const esAdmin = parseInt(user.rol_id) === 1;


    let navBar = `<nav class="navbar navbar-inverse">
                        <div class="container-fluid">
                            <div class="navbar-header">
                            <a class="navbar-brand" href="#"><span class="glyphicon glyphicon-tags"></span> eGesDoc</a>
                            </div>
                            <ul class="nav navbar-nav">
                            <li class="active"><a href="#" id="link-home"><span class="glyphicon glyphicon-home"></span> Home</a></li>
                            <li class="dropdown"><a class="dropdown-toggle" data-toggle="dropdown" href="#"><span class="glyphicon glyphicon-cog"></span> Sistema <span class="caret"></span></a>
                                <ul class="dropdown-menu">
                                <li><a href="#" id="link-usuarios" data-toggle="tooltip" title="Usuarios">
                                    <span class="glyphicon glyphicon-user"></span> Usuarios</a></li>
                                <li><a href="#" id="link-documentacion_tecnica" data-toggle="tooltip" title="Listar Documentación Técnica">
                                    <span class="glyphicon glyphicon-book"></span> Documentación Técnica</a></li>
                                <li><a href="#" id="link-tablas_maestro" data-toggle="tooltip" title="Listar Tablas Maestro">
                                    <span class="glyphicon glyphicon-th-list"></span> Tablas Maestro</a></li>
                                </ul>
                            </li>
                            <li><a href="#">Page 2</a></li>
                            </ul>

                            <ul class="nav navbar-nav navbar-right">
                            <li><a href="#" id="user-my-data" data-toggle="tooltip" title="Datos personales" data-id="${user.nombre}">
                                <span class="glyphicon glyphicon-user"></span> ${user.nombre}</a></li>
                            <li><a href="#" id="dashboard-logout" data-toggle="tooltip" title="Salir de la aplicación">
                                <span class="glyphicon glyphicon-log-out"></span> Salir</a></li>
                            </ul>
                        </div>
                    </nav><br>

                    <div class="container-fluid">
                        <div id="dashboard_messages"></div>

                            <div><br>
                                <div id="dashboard_views"></div>
                            </div>
                        </div>
                    </div>`;

    // Inyectar contenido en el contenedor
    const dashNav = document.getElementById("navBar");
    dashNav.innerHTML = navBar;

    cargarComboGenerosMusicales();

    // -------------------------------------------------------------------------
    // BÚSQUEDAS: ARTISTA Y GÉNERO
    // -------------------------------------------------------------------------
    window.albumFilter = null;

    function ejecutarFiltroAlbums(tipo, valor) {
      window.albumFilter = { tipo: tipo, valor: valor ? valor.trim() : "" };

      if (window.dTable && typeof window.filtrarTablaAlbums === "function") {
          window.filtrarTablaAlbums(window.albumFilter.tipo, window.albumFilter.valor);
      } else {
          loadDashboardView(window.VIEWS_PATH + "/albums/albums.html");
      }
    }

    const formArtist = document.getElementById("form_search_by_artist");
    if (formArtist) {
      formArtist.addEventListener("submit", (e) => {
        e.preventDefault();
        const artistaVal = document.getElementById("artista").value;
        ejecutarFiltroAlbums("artista", artistaVal);
      });
    }

    const formGenre = document.getElementById("form_search_by_genre");
    if (formGenre) {
      formGenre.addEventListener("submit", (e) => {
        e.preventDefault();
        const generoVal = document.getElementById("genres").value;
        ejecutarFiltroAlbums("genero", generoVal);
      });
    }

    document.addEventListener("click", (e) => {
      const btnEdit = e.target.closest("#user-my-data");
      if (btnEdit) {
        e.preventDefault();
        const userStored = JSON.parse(localStorage.getItem("user"));
        const user_name = userStored ? userStored.nombre : null;

        if (user_name) {
          loadDataUserForm(user_name);
        }
      }
    });
  }

  // ============================================================================================================================== //

  // Acción de logout
  const logoutBtn = document.getElementById("dashboard-logout");
  if (logoutBtn) {
    logoutBtn.addEventListener("click", () => {
      localStorage.removeItem("user");
      sessionStorage.clear();

      if (window.AimpMaster && typeof window.AimpMaster.stopAll === "function") {
        window.AimpMaster.stopAll();
      }

      console.log("🚪 Cerrando sesión...");
      var message = `<div class="container-fluid">
                        <div class="alert alert-info">
                            <p align="center"><span class="glyphicon glyphicon-exclamation-sign" aria-hidden="true"></span> <strong>Aguarde un instante!</strong> Estamos cerrando la sesión.</p>
                        </div>
                    </div>`;

      document.getElementById('dashboard_messages').innerHTML = message;
      setTimeout(function() { window.location.href = "/"; }, 3000);
    });
  }

  // Carga inicial
  loadDashboardView(window.VIEWS_PATH + "/billboard/billboard.html");

  // ======================================================================================================================== //
  // EVENTOS DE NAVEGACIÓN
  // ======================================================================================================================== //
  document.getElementById("link-usuarios")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/usuarios.html");
  });

  document.addEventListener("click", (e) => {
    const target = e.target.closest("#add-user-form");
    if (target) {
      e.preventDefault();
      loadDashboardView(window.VIEWS_PATH + "/register.html");
    }
  });

  document.addEventListener("click", (e) => {
    const target = e.target.closest('#editar_rol_usuario');
    if (target) {
      e.preventDefault();
      loadDashboardView(window.VIEWS_PATH + "/roles/rol_usuario.html");
    }
  });

  $("#nav-btn-billboard").on("click", function(e) {
    e.preventDefault();
    window.loadDashboardView(window.VIEWS_PATH + "/billboard/billboard.html");
  });

  document.getElementById("link-roles")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/roles/roles.html");
  });

  document.addEventListener("click", (e) => {
    const target = e.target.closest("#add-rol-form");
    if (target) {
      e.preventDefault();
      loadDashboardView(window.VIEWS_PATH + "/roles/nuevo_rol.html");
    }
  });

  document.getElementById("link-home")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/home.html");
  });

  document.getElementById("link-generos")?.addEventListener("click", (e) => {
    e.preventDefault();
    loadDashboardView(window.VIEWS_PATH + "/generos/generos.html");
  });

  // ======================================================================================================================== //
  // FUNCIÓN CENTRAL DESPACHADORA CON ADUANA PERIMETRAL
  // ======================================================================================================================== //
  async function loadDashboardView(viewPath) {
    const container = document.getElementById("dashboard_views");
    if (!container) return;

    const currentSession = JSON.parse(localStorage.getItem("user"));
    const rolUsuario = currentSession ? parseInt(currentSession.rol_id) : 0;

    const modulosAdministrativos = [
      "usuarios", "roles", "register", "rol_usuario", "nuevo_rol", "nuevo_modulo", "generos"
    ];

    const esRutaRestringida = modulosAdministrativos.some(keyword => viewPath.includes(keyword));

    // Cortafuegos de acceso a módulos administrativos
    if (esRutaRestringida && rolUsuario !== 1) {
      console.warn(`🛑 [CORTAFUEGOS]: Intento de acceso denegado a la ruta: ${viewPath}`);
      container.innerHTML = `
        <br>
        <div class="container-fluid">
          <div class="jumbotron" style="background-color: #fcf8e3; border: 1px solid #fbeed5; color: #c09853; border-radius: 6px;">
            <h2 class="text-danger">
              <span class="glyphicon glyphicon-lock" aria-hidden="true"></span> <strong>Acceso Restringido</strong>
            </h2>
            <hr style="border-top-color: #f7ecb5;">
            <p style="font-size: 16px;"><strong>Lo sentimos:</strong> Su usuario no tiene permisos de acceso a este módulo.</p>
            <p style="font-size: 14px;">Si cree que se trata de un error, por favor contacte al Administrador Supremo del Sistema.</p>
            <br>
            <button class="btn btn-warning" onclick="window.loadDashboardView('/views/home.html')">
              <span class="glyphicon glyphicon-home"></span> Volver al Inicio
            </button>
          </div>
        </div>
      `;
      return;
    }

    try {
      const response = await fetch(viewPath);
      if (!response.ok) throw new Error("Vista no encontrada");

      const html = await response.text();
      container.innerHTML = html;

      // Carga dinámica con la ruta física real: /views/js/...[cite: 9]
      if (viewPath.includes("usuarios")) {
        loadDashboardScript(window.VIEWS_PATH + "/js/usuarios.js");
      }
      else if (viewPath.includes("register")) {
        loadDashboardScript(window.VIEWS_PATH + "/js/register.js");
      }
      else if (viewPath.includes("password")) {
        loadDashboardScript(window.VIEWS_PATH + "/js/password.js");
      }
      else if (viewPath.includes("roles")) {
        loadDashboardScript(window.VIEWS_PATH + "/js/roles.js");
      }


      setTimeout(() => {
        if (typeof window.sincronizarCamposDeControl === "function") {
          window.sincronizarCamposDeControl();
        }
      }, 50);

    } catch (err) {
      console.error("Error al cargar vista:", err);
    }
  }

  // ======================================================================================================================== //

  function loadDashboardScript(src) {
    const scriptExistente = document.querySelector(`script[src="${src}"]`);
    if (scriptExistente) {
      scriptExistente.remove();
    }

    const script = document.createElement("script");
    script.type = "text/javascript";
    script.src = src;

    script.onerror = () => {
      console.error(`💥 Error al cargar el archivo de script: ${src}`);
    };

    document.body.appendChild(script);
  }

  // ======================================================================================================================== //

  // ======================================================================================================================== //
  // 🚀 DELEGACIÓN GLOBAL DE EVENTOS PARA NAVEGACIÓN ASÍNCRONA
  // ======================================================================================================================== //
  document.addEventListener("click", function (e) {
    // Buscamos si el clic se hizo en un ID de los que necesitamos capturar
    const targetLink = e.target.closest(
        "#link-tablas_maestro, \
         #link-instituciones, \
         #link-actividades, \
         #link-agrupamientos, \
         #link-cargos_directivos, \
         #link-discapacidades, \
         #link-disciplinas_estudio, \
         #link-escalafones_completos , \
         #link-estado_civil, \
         #link-fuente_financiamiento, \
         #link-identidad_genero, \
         #link-jurisdicciones, \
         #link-marca_estado, \
         #link-nacionalidades, \
         #link-nivel_educativo, \
         #link-niveles, \
         #link-programas, \
         #link-proyectos, \
         #link-remunerativos_bonificables, \
         #link-sanciones_disciplinarias, \
         #link-sexos, \
         #link-subjurisdicciones, \
         #link-subprogramas, \
         #link-tipo_accesos, \
         #link-tipo_conceptos, \
         #link-tipo_documentos, \
         #link-tipo_horarios, \
         #link-tipo_licencias, \
         #link-tipo_plantas, \
         #link-tipo_unidades_fisicas, \
         #link-ubicaciones_geograficas, \
         #link-unidades_organizativas, \
         #link-entidades, \
         #doc_backend_framework_auth, \
         #doc_backend_framework_backend_server, \
         #doc_backend_framework_commonlib, \
         #doc_backend_framework_db, \
         #doc_backend_framework_handle, \
         #doc_backend_framework_hash, \
         #doc_backend_framework_login, \
         #doc_backend_framework_routes, \
         #doc_backend_framework_session_manager, \
         #doc_backend_framework_system_struct, \
         #doc_backend_framework_users, \
         #doc_backend_framework_arraylist, \
         #doc_backend_framework_frontend_server, \
         #doc_backend_bussiness_logic_actividades_service, \
         #doc_backend_bussiness_logic_agrupamiento_service, \
         #doc_backend_bussiness_logic_cargo_directivo_service, \
         #doc_backend_bussiness_logic_ch_service, \
         #doc_backend_bussiness_logic_copiar_lotes_service, \
         #doc_backend_bussiness_logic_discapacidades_service, \
         #doc_backend_bussiness_logic_disciplina_estudio_service, \
         #doc_backend_bussiness_logic_dp_service, \
         #doc_backend_bussiness_logic_eliminar_lote_service, \
         #doc_backend_bussiness_logic_entidades_service, \
         #doc_backend_bussiness_logic_escalafones_service, \
         #doc_backend_bussiness_logic_estado_civil_service, \
         #doc_backend_bussiness_logic_estado_lote_service, \
         #doc_backend_bussiness_logic_fuente_financiamiento_service, \
         #doc_backend_bussiness_logic_generador_lotes_service, \
         #doc_backend_bussiness_logic_identidad_genero_service, \
         #doc_backend_bussiness_logic_instituciones_service, \
         #doc_backend_bussiness_logic_jurisdicciones_service, \
         #doc_backend_bussiness_logic_lh1_service, \
         #doc_backend_bussiness_logic_lh2_service, \
         #doc_backend_bussiness_logic_marca_estado_service, \
         #doc_backend_bussiness_logic_nacionalidades_service, \
         #doc_backend_bussiness_logic_nivel_educativo_service, \
         #doc_backend_bussiness_logic_niveles_service, \
         #doc_backend_bussiness_logic_parametros_basicos_service, \
         #doc_backend_bussiness_logic_programas_service, \
         #doc_backend_bussiness_logic_proyectos_service, \
         #doc_backend_bussiness_logic_remunerativo_bonificable_service, \
         #doc_backend_bussiness_logic_sancion_disciplinaria_service, \
         #doc_backend_bussiness_logic_sexo_service, \
         #doc_backend_bussiness_logic_subjurisdicciones_service, \
         #doc_backend_bussiness_logic_subprogramas_service, \
         #doc_backend_bussiness_logic_tipo_acceso_service, \
         #doc_backend_bussiness_logic_tipo_concepto_service, \
         #doc_backend_bussiness_logic_tipo_documento_service, \
         #doc_backend_bussiness_logic_tipo_horario_service, \
         #doc_backend_bussiness_logic_tipo_licencia_service, \
         #doc_backend_bussiness_logic_tipo_planta_service, \
         #doc_backend_bussiness_logic_tipo_unidad_fisica_service, \
         #doc_backend_bussiness_logic_ubicacion_geografica_service, \
         #doc_backend_bussiness_logic_unidad_organizativa_service, \
         #doc_backend_bussiness_logic_users_service, \
         #doc_backend_bussiness_logic_validacion_lote_service, \
         #doc_backend_bussiness_logic_verificar_externos_service, \
         #doc_database_schema, \
         #doc_db_gls_usuarios, \
         #doc_db_gls_roles, \
         #doc_db_gls_ch, \
         #doc_db_gls_dp, \
         #doc_db_gls_lh1, \
         #doc_db_gls_lh2, \
         #doc_db_gls_parametros, \
         #doc_db_gls_instituciones, \
         #doc_db_gls_jurisdicciones, \
         #doc_db_gls_sub_jurisdicciones, \
         #doc_db_gls_entidades, \
         #doc_db_gls_programas, \
         #doc_db_gls_subprogramas, \
         #doc_db_gls_proyectos, \
         #doc_db_gls_actividades, \
         #doc_db_gls_unidad_organizativa, \
         #doc_db_gls_ubicacion_geografica, \
         #doc_db_gls_permisos_usuarios, \
         #doc_db_gls_modulos, \
         #doc_db_gls_tipo_documento, \
         #doc_db_gls_identidad_genero, \
         #doc_db_gls_estado_civil, \
         #doc_db_gls_nacionalidad, \
         #doc_db_gls_codigo_discapacidad, \
         #doc_db_gls_codigo_disciplina_estudio, \
         #doc_db_gls_tipo_concepto, \
         #doc_db_gls_remunerativo_bonificable, \
         #doc_db_gls_tipo_unidad_fisica, \
         #doc_db_gls_fuente_financiamiento, \
         #doc_db_gls_tipo_planta, \
         #doc_db_gls_tipo_licencia, \
         #doc_db_gls_sancion_disciplinaria, \
         #doc_db_gls_tipo_horario, \
         #doc_db_gls_tipo_acceso, \
         #doc_db_gls_sexo, \
         #doc_db_gls_nivel_educativo");

    if (targetLink) {
      e.preventDefault(); // Evitamos que la página intente recargarse o saltar

      const viewId = targetLink.id;
      console.log(`🎯 Navegación interceptada dinámicamente desde: ${viewId}`);

      // Despachamos de forma limpia al contenedor principal según el ID del link
      switch (viewId) {
        case "link-tablas_maestro":
          loadDashboardView(window.VIEWS_PATH + "/tablas_maestro.html");
          break;
        case "link-instituciones":
          loadDashboardView(window.VIEWS_PATH + "/instituciones/instituciones.html");
          break;
        case "link-actividades":
          loadDashboardView(window.VIEWS_PATH + "/actividades/actividades.html");
          break;
        case "link-agrupamientos":
          loadDashboardView(window.VIEWS_PATH + "/agrupamientos/agrupamientos.html");
          break;
        case "link-cargos_directivos":
          loadDashboardView(window.VIEWS_PATH + "/cargos_directivos/cargos_directivos.html");
          break;
        case "link-discapacidades":
          loadDashboardView(window.VIEWS_PATH + "/discapacidades/discapacidades.html");
          break;
        case "link-disciplinas_estudio":
          loadDashboardView(window.VIEWS_PATH + "/disciplinas_estudio/disciplinas.html");
          break;
        case "link-escalafones_completos":
          loadDashboardView(window.VIEWS_PATH + "/escalafones/escalafones.html");
          break;
        case "link-estado_civil":
          loadDashboardView(window.VIEWS_PATH + "/estado_civil/estado_civil.html");
          break;
        case "link-fuente_financiamiento":
          loadDashboardView(window.VIEWS_PATH + "/fuente_financiamiento/fuente_financiamiento.html");
          break;
        case "link-identidad_genero":
          loadDashboardView(window.VIEWS_PATH + "/identidad_genero/identidad_genero.html");
          break;
        case "link-jurisdicciones":
          loadDashboardView(window.VIEWS_PATH + "/jurisdicciones/jurisdicciones.html");
          break;
        case "link-marca_estado":
          loadDashboardView(window.VIEWS_PATH + "/marca_estado/marca_estado.html");
          break;
        case "link-nacionalidades":
          loadDashboardView(window.VIEWS_PATH + "/nacionalidades/nacionalidades.html");
          break;
        case "link-nivel_educativo":
          loadDashboardView(window.VIEWS_PATH + "/nivel_educativo/nivel_educativo.html");
          break;
        case "link-niveles":
          loadDashboardView(window.VIEWS_PATH + "/niveles/niveles.html");
          break;
        case "link-programas":
          loadDashboardView(window.VIEWS_PATH + "/programas/programas.html");
          break;
        case "link-proyectos":
          loadDashboardView(window.VIEWS_PATH + "/proyectos/proyectos.html");
          break;
        case "link-remunerativos_bonificables":
          loadDashboardView(window.VIEWS_PATH + "/remunerativos_bonificables/remunerativo_bonificable.html");
          break;
        case "link-sanciones_disciplinarias":
          loadDashboardView(window.VIEWS_PATH + "/sancion_disciplinaria/sanciones_disciplinarias.html");
          break;
        case "link-sexos":
          loadDashboardView(window.VIEWS_PATH + "/sexos/sexos.html");
          break;
        case "link-subjurisdicciones":
          loadDashboardView(window.VIEWS_PATH + "/subjurisdicciones/subjurisdicciones.html");
          break;
        case "link-subprogramas":
          loadDashboardView(window.VIEWS_PATH + "/subprogramas/subprogramas.html");
          break;
        case "link-tipo_accesos":
          loadDashboardView(window.VIEWS_PATH + "/tipo_acceso/tipo_accesos.html");
          break;
        case "link-tipo_conceptos":
          loadDashboardView(window.VIEWS_PATH + "/tipo_concepto/tipo_conceptos.html");
          break;
        case "link-tipo_documentos":
          loadDashboardView(window.VIEWS_PATH + "/tipo_documento/tipo_documentos.html");
          break;
        case "link-tipo_horarios":
          loadDashboardView(window.VIEWS_PATH + "/tipo_horario/tipo_horarios.html");
          break;
        case "link-tipo_licencias":
          loadDashboardView(window.VIEWS_PATH + "/tipo_licencia/tipo_licencias.html");
          break;
        case "link-tipo_plantas":
          loadDashboardView(window.VIEWS_PATH + "/tipo_planta/tipo_plantas.html");
          break;
        case "link-tipo_unidades_fisicas":
          loadDashboardView(window.VIEWS_PATH + "/tipo_unidad_fisica/tipo_unidades_fisicas.html");
          break;
        case "link-ubicaciones_geograficas":
          loadDashboardView(window.VIEWS_PATH + "/ubicacion_geografica/ubicaciones_geograficas.html");
          break;
        case "link-unidades_organizativas":
          loadDashboardView(window.VIEWS_PATH + "/unidad_organizativa/unidades_organizativas.html");
          break;
        case "link-entidades":
          loadDashboardView(window.VIEWS_PATH + "/entidades/entidades.html");
          break;
        case "doc_backend_framework_auth":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_auth.html");
          break;
        case "doc_backend_framework_backend_server":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_backend_server.html");
          break;
        case "doc_backend_framework_commonlib":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_commonlib.html");
          break;
        case "doc_backend_framework_db":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_db.html");
          break;
        case "doc_backend_framework_handle":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_handle.html");
          break;
        case "doc_backend_framework_hash":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_hash.html");
          break;
        case "doc_backend_framework_login":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_login.html");
          break;
        case "doc_backend_framework_routes":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_routes.html");
          break;
        case "doc_backend_framework_session_manager":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_session_manager.html");
          break;
        case "doc_backend_framework_system_struct":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_system_struct.html");
          break;
        case "doc_backend_framework_users":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_users.html");
          break;
        case "doc_backend_framework_arraylist":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_arraylist.html");
          break;
        case "doc_backend_framework_frontend_server":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_framework/doc_backend_framework_frontend_server.html");
          break;
        case "doc_backend_bussiness_logic_actividades_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_actividades_service.html");
          break;
        case "doc_backend_bussiness_logic_agrupamiento_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_agrupamiento_service.html");
          break;
        case "doc_backend_bussiness_logic_cargo_directivo_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_cargo_directivo_service.html");
          break;
        case "doc_backend_bussiness_logic_ch_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_ch_service.html");
          break;
        case "doc_backend_bussiness_logic_dp_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_dp_service.html");
          break;
        case "doc_backend_bussiness_logic_copiar_lotes_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_copiar_lotes_service.html");
          break;
        case "doc_backend_bussiness_logic_discapacidades_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_discapacidades_service.html");
          break;
        case "doc_backend_bussiness_logic_disciplina_estudio_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_disciplina_estudio_service.html");
          break;
        case "doc_backend_bussiness_logic_eliminar_lote_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_eliminar_lote_service.html");
          break;
        case "doc_backend_bussiness_logic_entidades_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_entidades_service.html");
          break;
        case "doc_backend_bussiness_logic_escalafones_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_escalafones_service.html");
          break;
        case "doc_backend_bussiness_logic_estado_civil_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_estado_civil_service.html");
          break;
        case "doc_backend_bussiness_logic_estado_lote_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_estado_lote_service.html");
          break;
        case "doc_backend_bussiness_logic_fuente_financiamiento_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_fuente_financiamiento_service.html");
          break;
        case "doc_backend_bussiness_logic_generador_lotes_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_generador_lotes_service.html");
          break;
        case "doc_backend_bussiness_logic_identidad_genero_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_identidad_genero_service.html");
          break;
        case "doc_backend_bussiness_logic_instituciones_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_instituciones_service.html");
          break;
        case "doc_backend_bussiness_logic_jurisdicciones_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_jurisdicciones_service.html");
          break;
        case "doc_backend_bussiness_logic_lh1_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_lh1_service.html");
          break;
        case "doc_backend_bussiness_logic_lh2_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_lh2_service.html");
          break;
        case "doc_backend_bussiness_logic_marca_estado_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_marca_estado_service.html");
          break;
        case "doc_backend_bussiness_logic_nacionalidades_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_nacionalidades_service.html");
          break;
        case "doc_backend_bussiness_logic_nivel_educativo_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_nivel_educativo_service.html");
          break;
        case "doc_backend_bussiness_logic_niveles_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_niveles_service.html");
          break;
        case "doc_backend_bussiness_logic_parametros_basicos_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_parametros_basicos_service.html");
          break;
        case "doc_backend_bussiness_logic_programas_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_programas_service.html");
          break;
        case "doc_backend_bussiness_logic_proyectos_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_proyectos_service.html");
          break;
        case "doc_backend_bussiness_logic_remunerativo_bonificable_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_remunerativo_bonificable_service.html");
          break;
        case "doc_backend_bussiness_logic_sancion_disciplinaria_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_sancion_disciplinaria_service.html");
          break;
        case "doc_backend_bussiness_logic_sexo_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_sexo_service.html");
          break;
        case "doc_backend_bussiness_logic_subjurisdicciones_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_subjurisdicciones_service.html");
          break;
        case "doc_backend_bussiness_logic_subprogramas_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_subprogramas_service.html");
          break;
        case "doc_backend_bussiness_logic_tipo_acceso_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_acceso_service.html");
          break;
        case "doc_backend_bussiness_logic_tipo_concepto_service":
          loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_concepto_service.html");
          break;
        case "doc_backend_bussiness_logic_tipo_documento_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_documento_service.html");
            break;
        case "doc_backend_bussiness_logic_tipo_horario_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_horario_service.html");
            break;
        case "doc_backend_bussiness_logic_tipo_licencia_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_licencia_service.html");
            break;
        case "doc_backend_bussiness_logic_tipo_planta_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_planta_service.html");
            break;
        case "doc_backend_bussiness_logic_tipo_unidad_fisica_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_tipo_unidad_fisica_service.html");
            break;
        case "doc_backend_bussiness_logic_ubicacion_geografica_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_ubicacion_geografica_service.html");
            break;
        case "doc_backend_bussiness_logic_unidad_organizativa_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_unidad_organizativa_service.html");
            break;
        case "doc_backend_bussiness_logic_users_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_users_service.html");
            break;
        case "doc_backend_bussiness_logic_validacion_lote_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_validacion_lote_service.html");
            break;
        case "doc_backend_bussiness_logic_verificar_externos_service":
            loadDashboardView(window.VIEWS_PATH + "/doc_backend_bussiness_logic/doc_backend_bussiness_logic_verificar_externos_service.html");
            break;
        case "doc_database_schema":
            loadDashboardView(window.VIEWS_PATH + "/doc_database_schema.html");
            break;
        case "doc_db_gls_usuarios":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_usuarios.html");
            break;
        case "doc_db_gls_roles":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_roles.html");
            break;
        case "doc_db_gls_ch":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_ch.html");
            break;
        case "doc_db_gls_dp":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_dp.html");
            break;
        case "doc_db_gls_lh1":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_lh1.html");
            break;
        case "doc_db_gls_lh2":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_lh2.html");
            break;
        case "doc_db_gls_parametros":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_parametros.html");
            break;
        case "doc_db_gls_instituciones":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_instituciones.html");
            break;
        case "doc_db_gls_jurisdicciones":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_jurisdicciones.html");
            break;
        case "doc_db_gls_sub_jurisdicciones":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_sub_jurisdicciones.html");
            break;
        case "doc_db_gls_entidades":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_entidades.html");
            break;
        case "doc_db_gls_programas":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_programas.html");
            break;
        case "doc_db_gls_subprogramas":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_subprogramas.html");
            break;
        case "doc_db_gls_proyectos":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_proyectos.html");
            break;
        case "doc_db_gls_actividades":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_actividades.html");
            break;
        case "doc_db_gls_unidad_organizativa":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_unidad_organizativa.html");
            break;
        case "doc_db_gls_ubicacion_geografica":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_ubicacion_geografica.html");
            break;
        case "doc_db_gls_permisos_usuarios":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_permisos_usuarios.html");
            break;
        case "doc_db_gls_modulos":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_modulos.html");
            break;
        case "doc_db_gls_tipo_documento":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_documento.html");
            break;
        case "doc_db_gls_identidad_genero":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_identidad_genero.html");
            break;
        case "doc_db_gls_estado_civil":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_estado_civil.html");
            break;
        case "doc_db_gls_nacionalidad":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_nacionalidad.html");
            break;
        case "doc_db_gls_codigo_discapacidad":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_codigo_discapacidad.html");
            break;
        case "doc_db_gls_codigo_disciplina_estudio":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_codigo_disciplina_estudio.html");
            break;
        case "doc_db_gls_tipo_concepto":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_concepto.html");
            break;
        case "doc_db_gls_remunerativo_bonificable":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_remunerativo_bonificable.html");
            break;
        case "doc_db_gls_tipo_unidad_fisica":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_unidad_fisica.html");
            break;
        case "doc_db_gls_fuente_financiamiento":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_fuente_financiamiento.html");
            break;
        case "doc_db_gls_tipo_planta":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_planta.html");
            break;
        case "doc_db_gls_tipo_licencia":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_licencia.html");
            break;
        case "doc_db_gls_sancion_disciplinaria":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_sancion_disciplinaria.html");
            break;
        case "doc_db_gls_tipo_horario":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_horario.html");
            break;
        case "doc_db_gls_tipo_acceso":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_tipo_acceso.html");
            break;
        case "doc_db_gls_sexo":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_sexo.html");
            break;
        case "doc_db_gls_nivel_educativo":
            loadDashboardView(window.VIEWS_PATH + "/doc_db/doc_db_gls_nivel_educativo.html");
            break;



        default:
          console.warn("⚠️ ID de navegación no mapeado en el enrutador central.");
          break;
      }
    }
  });

  window.loadDashboardView = loadDashboardView;
});

// ====================================================================================================================== //

async function cargarComboGenerosMusicales(valorSeleccionado = "") {
    const select = document.getElementById("genres");
    if (!select) return;

    try {
        const resp = await fetch(window.API_BASE_URL + "/genres/list");
        const lista = await resp.json();

        select.innerHTML = `<option value="">-- Todos los Géneros --</option>`;
        lista.forEach(item => {
            const genre = item.genre ? item.genre.trim() : "";
            if (!genre) return;
            const opt = document.createElement("option");
            opt.value = genre;
            opt.textContent = genre;
            if (genre === valorSeleccionado) opt.selected = true;
            select.appendChild(opt);
        });
    } catch (err) {
        console.error("Error al cargar Géneros Musicales en el sidebar:", err);
        select.innerHTML = `<option value="">-- Error cargando géneros --</option>`;
    }
}

// ====================================================================================================================== //

async function loadDataUserForm(userName) {
  const container = document.getElementById("dashboard_views");

  try {
    const responseView = await fetch(window.VIEWS_PATH + "/user_data.html");
    if (!responseView.ok) throw new Error(`HTTP ${responseView.status}`);
    container.innerHTML = await responseView.text();

    const responseData = await fetch(window.API_BASE_URL + "/users/get-user", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: `user_name=${encodeURIComponent(userName)}`
    });

    if (!responseData.ok) throw new Error("No se pudo obtener la data del usuario");

    const text = await responseData.text();
    const data = JSON.parse(text);

    setTimeout(() => {
      if (data) {
        if (document.getElementById("id")) document.getElementById("id").value = data.id || "";
        if (document.getElementById("nombre")) document.getElementById("nombre").value = data.nombre || "";
        if (document.getElementById("email")) document.getElementById("email").value = data.email || "";
        if (document.getElementById("rol")) document.getElementById("rol").value = data.rol || "";
      }
    }, 150);

    const form = document.getElementById("user_data_form");
    if (form) {
      form.onsubmit = async (e) => {
        e.preventDefault();

        const formData = new URLSearchParams(new FormData(form));
        formData.set("email", document.getElementById("email").value);
        formData.set("password_1", document.getElementById("password_1").value);
        formData.set("password_2", document.getElementById("password_2").value);

        const res = await fetch(window.API_BASE_URL + "/password", {
          method: "POST",
          headers: { "Content-Type": "application/x-www-form-urlencoded" },
          body: formData.toString()
        });

        const result = await res.json();
        const msgDiv = document.getElementById('msg-update-data-user');

        if (result.status === "ok") {
          msgDiv.innerHTML = `<br><div class="alert alert-success alert-dismissible">
                                <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
                                <p align=center><span class="glyphicon glyphicon-ok"></span> ${result.message}</p></div>`;
          setTimeout(() => { $(".close").click(); }, 3000);
          setTimeout(() => { loadDashboardView(window.VIEWS_PATH + "/home.html"); }, 4000);
        } else {
          msgDiv.innerHTML = `<br><div class="alert alert-danger alert-dismissible">
                                <a href="#" class="close" data-dismiss="alert" aria-label="close">&times;</a>
                                <p align=center><span class="glyphicon glyphicon-warning-sign"></span> ${result.message}</p></div>`;
          setTimeout(() => { $(".close").click(); }, 3000);
        }
      };
    }

  } catch (error) {
    console.error("💥 Error en loadDataUserForm:", error);
  }
}

window.loadDataUserForm = loadDataUserForm;
