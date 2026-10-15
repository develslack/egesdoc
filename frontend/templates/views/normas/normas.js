console.clear();
console.log("✅ Módulo Normas Iniciado de forma nativa.");

window.dTable = window.dTable || null;

// ===================================================================================================================== //
// FUNCIÓN AUXILIAR: REEMPLAZO DE ALERT() POR MODAL DINÁMICO
// ===================================================================================================================== //
function mostrarAlerta(mensaje, titulo = "Atención", esHtml = false) {
    let $alertaModal =$("#custom-alert-modal");

    if ($alertaModal.length === 0) {
        const modalHtml = `
        <div class="modal fade" id="custom-alert-modal" tabindex="-1" role="dialog" style="z-index: 11000;">
          <div class="modal-dialog modal-sm" role="document">
            <div class="modal-content" style="background-color: #1e1e24; color: #e0e0e0; border: 1px solid #3f424e; border-radius: 8px;">
              <div class="modal-header" style="border-bottom: 1px solid #3f424e;">
                <button type="button" class="close" data-dismiss="modal" aria-label="Close" style="color: #fff; opacity: 0.8; text-shadow: none;"><span aria-hidden="true">&times;</span></button>
                <h4 class="modal-title" style="color: #5dade2; font-weight: bold;">
                    <span class="glyphicon glyphicon-info-sign"></span> <span id="custom-alert-title"></span>
                </h4>
              </div>
              <div class="modal-body">
                <div id="custom-alert-message" style="font-size: 1.05em; margin-bottom: 0;"></div>
              </div>
              <div class="modal-footer" style="border-top: none; padding-top: 0;">
                <button type="button" class="btn btn-info btn-sm" data-dismiss="modal">Aceptar</button>
              </div>
            </div>
          </div>
        </div>`;
        $("body").append(modalHtml);
        $alertaModal =$("#custom-alert-modal");
    }

    $("#custom-alert-title").text(titulo);
    if (esHtml) {
        $("#custom-alert-message").html(mensaje);
    } else {
        $("#custom-alert-message").text(mensaje);
    }
    $alertaModal.modal("show");
}

(async () => {
  try {
    // ================================================================================================================= //
    // 1. CARGA DE LA TABLA MAESTRA DESDE EL BACKEND EN C
    // ================================================================================================================= //
    const response = await fetch(window.API_BASE_URL +"/normas/list");
    const registros = await response.json();

    const tableBody = document.getElementById("normasTableBody");
    const alertInfo = document.getElementById("alert-info-normas");
    tableBody.innerHTML = "";

    let count = 0;

    registros.forEach((reg) => {
      const row = document.createElement("tr");
      row.innerHTML = `
        <td class="text-center">${reg.nombre_norma}</td>
        <td class="text-center">${reg.n_norma}</td>
        <td class="text-center">${reg.tipo_norma}</td>
        <td class="text-center">${reg.f_norma}</td>
        <td class="text-center">${reg.f_pub}</td>
        <td class="text-center">${reg.anio_pub}</td>
        <td class="text-center">${reg.jurisdiccion}</td>
        <td class="text-center">${reg.organismo}</td>
        <td class="text-center">${reg.descripcion_organismo}</td>
        <td class="text-center">${reg.ubicacion_fisica}</td>
        <td class="text-center" style="white-space: nowrap;">
          <button class="btn btn-default btn-sm btn-editar-norma"
                  data-id="${reg.id}"
                  data-nombre_norma="${reg.nombre_norma}"
                  data-n_norma="${reg.n_norma}"
                  data-tipo_norma="${reg.tipo_norma}"
                  data-f_norma="${reg.f_norma}"
                  data-f_pub="${reg.f_pub}"
                  data-anio_pub="${reg.anio_pub}"
                  data-jurisdiccion="${reg.jurisdiccion}"
                  data-organismo="${reg.organismo}"
                  data-ubicacion_fisica="${reg.ubicacion_fisica}"
                  data-observaciones="${reg.observaciones}"
                  data-file_name="${reg.file_name}">
            <span class="glyphicon glyphicon-edit"></span> Editar
          </button>
          <button class="btn btn-default btn-sm btn-ver-doc" data-id="${reg.id}">
            <span class="glyphicon glyphicon-eye-open"></span> Ver Doc
          </button>
          <button class="btn btn-info btn-sm btn-informe_extendido"
                  data-id="${reg.id}"
                  data-nombre_norma="${reg.nombre_norma}"
                  data-n_norma="${reg.n_norma}"
                  data-tipo_norma="${reg.tipo_norma}"
                  data-f_norma="${reg.f_norma}"
                  data-f_pub="${reg.f_pub}"
                  data-anio_pub="${reg.anio_pub}"
                  data-jurisdiccion="${reg.jurisdiccion}"
                  data-organismo="${reg.organismo}"
                  data-descripcion_organismo="${reg.descripcion_organismo}"
                  data-ubicacion_fisica="${reg.ubicacion_fisica}"
                  data-observaciones="${reg.observaciones}"
                  data-file_name="${reg.file_name}">
            <span class="glyphicon glyphicon-list-alt"></span> Información Extendida
          </button>
        </td>`;

      tableBody.appendChild(row);
      count++;
    });

    if (alertInfo) {
        alertInfo.innerHTML = `<div class="alert alert-info">
                                <span class="glyphicon glyphicon-option-vertical" aria-hidden="true"></span> <strong>Cantidad de Registros: </strong> ${count}
                               </div><hr>`;
    }

    if ($.fn.DataTable.isDataTable("#normasTable")) {
      $("#normasTable").DataTable().destroy();
    }

    dTable = $('#normasTable').DataTable({
        "order": [[0, "asc"]],
        "responsive":     true,
        "scrollY":        "300px",
        "scrollX":        true,
        "scrollCollapse": true,
        "paging":         true,
        "deferRender":    true,
        "retrieve":       true,
        "dom": '<"row"<"col-sm-12"l>><"row"<"col-sm-12"Bf>>rt<"row"<"col-sm-12"ip>>',
        buttons: [
            { extend: 'excel', text: '<i class="glyphicon glyphicon-list-alt"></i> Excel', exportOptions: { columns: ':visible'} },
            { extend: 'pdf', text: '<i class="glyphicon glyphicon-file"></i> PDF', exportOptions: { columns: ':visible'} },
            'colvis'
        ],
        "language": {
            "lengthMenu": "Mostrar _MENU_ registros por pagina",
            "info": "Mostrando pagina _PAGE_ de _PAGES_",
            "search": "Buscar:",
            "zeroRecords":    "No se encontraron registros coincidentes",
            "paginate": { "next": "Siguiente", "previous": "Anterior" }
        }
    });

    // ================================================================================================================= //
    // 2. PREPARACIÓN DEL MODAL DE ALTA/EDICION
    // ================================================================================================================= //
    const contenedorModal = document.getElementById("contenedor-modal-maestro-normas");
    if (contenedorModal) {
        const respModal = await fetch(window.VIEWS_PATH + "/normas/nueva_norma.html");
        contenedorModal.innerHTML = await respModal.text();
    }

    // ================================================================================================================= //
    // 3. CAPTURA SEGURO DE EVENTOS
    // ================================================================================================================= //

    $(document).off("click", "#add-norma-form").on("click", "#add-norma-form", function(e) {
        e.preventDefault();
        prepararYMostrarModal("alta");
    });

    $(document).off("click", ".btn-editar-norma").on("click", ".btn-editar-norma", function(e) {
        e.preventDefault();
        const datos = {
            id: $(this).attr("data-id"),
            nombre_norma: $(this).attr("data-nombre_norma"),
            n_norma: $(this).attr("data-n_norma"),
            tipo_norma: $(this).attr("data-tipo_norma"),
            f_norma: $(this).attr("data-f_norma"),
            f_pub: $(this).attr("data-f_pub"),
            anio_pub: $(this).attr("data-anio_pub"),
            jurisdiccion: $(this).attr("data-jurisdiccion"),
            organismo: $(this).attr("data-organismo"),
            ubicacion_fisica: $(this).attr("data-ubicacion_fisica"),
            observaciones: $(this).attr("data-observaciones"),
            file_name: $(this).attr("data-file_name")
        };
        prepararYMostrarModal("edicion", datos);
    });

    // Evento Ver Documento PDF Principal
    $(document).off("click", ".btn-ver-doc").on("click", ".btn-ver-doc", function(e) {
        e.preventDefault();
        const docId = $(this).attr("data-id");
        const nombreNorma = $(this).closest("tr").find("td:eq(0)").text();
        const endpoint = window.API_BASE_URL + "/normas/open_doc";
        const titulo = `Norma: ${nombreNorma}`;
        window.verDocumentoGlobal(endpoint, docId, titulo);
    });

    // Evento Información Extendida
    $(document).off("click", ".btn-informe_extendido").on("click", ".btn-informe_extendido", function(e) {
        e.preventDefault();
        const datos = {
            id: $(this).attr("data-id"),
            nombre_norma: $(this).attr("data-nombre_norma"),
            n_norma: $(this).attr("data-n_norma"),
            tipo_norma: $(this).attr("data-tipo_norma"),
            f_norma: $(this).attr("data-f_norma"),
            f_pub: $(this).attr("data-f_pub"),
            anio_pub: $(this).attr("data-anio_pub"),
            jurisdiccion: $(this).attr("data-jurisdiccion"),
            organismo: $(this).attr("data-organismo"),
            descripcion_organismo: $(this).attr("data-descripcion_organismo"),
            ubicacion_fisica: $(this).attr("data-ubicacion_fisica"),
            observaciones: $(this).attr("data-observaciones"),
            file_name: $(this).attr("data-file_name")
        };
        mostrarInformeExtendido(datos);
    });

  } catch (error) {
    console.error("💥 Error general en el módulo Normas:", error);
  }
})();

// ===================================================================================================================== //
// FUNCIÓN QUE CONSTRUYE EL INFORME EXTENDIDO (MODAL FLOTANTE)
// ===================================================================================================================== //
function mostrarInformeExtendido(datos) {
    let $modal =$("#modal-informe-extendido");

    if ($modal.length === 0) {
        const modalHtml = `
        <style>
            .ext-info-row {
                display: flex;
                justify-content: space-between;
                align-items: center;
                padding: 12px 15px;
                border-bottom: 1px solid #3f424e;
                background-color: #2b2d35;
            }
            .ext-info-row:nth-child(even) { background-color: #26282e; }
            .ext-info-label { font-weight: bold; color: #e0e0e0; }
            .ext-info-badge { background-color: #5dade2; color: #fff; padding: 4px 12px; border-radius: 20px; font-weight: bold; font-size: 0.9em; text-align: right; }
            .ext-info-obs { padding: 15px; background-color: #26282e; color: #c0c0c0; border-bottom: 1px solid #3f424e; font-size: 0.95em; }
            .ext-btn-row { padding: 15px; text-align: center; background-color: #1e1e24; display: flex; flex-direction: column; gap: 8px; }
            .ext-btn-row .btn { width: 100%; border: 1px solid #3f424e; background-color: #2b2d35; color: #e0e0e0; font-weight: bold; transition: all 0.3s; }
            .ext-btn-row .btn:hover { background-color: #3f424e; color: #fff; }
        </style>
        <div class="modal fade" id="modal-informe-extendido" tabindex="-1" role="dialog" style="z-index: 10500;">
          <div class="modal-dialog modal-md" role="document">
            <div class="modal-content" style="background-color: #1e1e24; border: 1px solid #3f424e; border-radius: 8px;">
              <div class="modal-header" style="border-bottom: 1px solid #3f424e; background-color: #2b2d35; border-radius: 8px 8px 0 0;">
                <button type="button" class="close" data-dismiss="modal" aria-label="Close" style="color: #fff; opacity: 0.8;"><span aria-hidden="true">&times;</span></button>
                <h4 class="modal-title" style="color: #5dade2; font-weight: bold;">
                    <span class="glyphicon glyphicon-tasks"></span> Información Extendida
                </h4>
              </div>
              <div class="modal-body" style="padding: 0;" id="ext-info-content">
                <!-- Contenido dinámico -->
              </div>
            </div>
          </div>
        </div>`;
        $("body").append(modalHtml);
        $modal =$("#modal-informe-extendido");
    }

    const htmlContent = `
        <div class="ext-info-row">
            <span class="ext-info-label">Nombre de la Norma:</span>
            <span class="ext-info-badge">${datos.nombre_norma}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Número de la Norma:</span>
            <span class="ext-info-badge">${datos.n_norma}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Tipo de Norma:</span>
            <span class="ext-info-badge">${datos.tipo_norma}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Ámbito de la Norma:</span>
            <span class="ext-info-badge">${datos.f_norma}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Fecha Publicación:</span>
            <span class="ext-info-badge">${datos.f_pub}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Año Publicación:</span>
            <span class="ext-info-badge">${datos.anio_pub}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Organismo:</span>
            <span class="ext-info-badge">${datos.descripcion_organismo || datos.organismo}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Jurisdicción:</span>
            <span class="ext-info-badge">${datos.jurisdiccion}</span>
        </div>
        <div class="ext-info-row">
            <span class="ext-info-label">Ubicación / Bibliorato:</span>
            <span class="ext-info-badge" style="background-color: #7f8c8d;">${datos.ubicacion_fisica || 'no tiene'}</span>
        </div>
        <div class="ext-info-obs">
            <strong>Observaciones:</strong> ${datos.observaciones}
        </div>

        <div class="ext-btn-row">
            <button class="btn btn-default" data-dismiss="modal">
                <span class="glyphicon glyphicon-arrow-left"></span> Volver a Normas
            </button>
            <button class="btn btn-default" id="btn-ver-vinculadas" data-id="${datos.id}">
                <span class="glyphicon glyphicon-duplicate text-info"></span> Ver Normas Vinculadas
            </button>
            <button class="btn btn-default" id="btn-imprimir-informe">
                <span class="glyphicon glyphicon-print"></span> Imprimir Informe
            </button>
            <button class="btn btn-default" id="btn-ver-doc-principal" data-id="${datos.id}" data-nombre="${datos.nombre_norma}">
                <span class="glyphicon glyphicon-eye-open text-success"></span> Ver Documento Principal
            </button>
        </div>
    `;

    $("#ext-info-content").html(htmlContent);
    $modal.modal("show");

    // Eventos internos del modal de Información Extendida
    $("#btn-ver-doc-principal").off("click").on("click", function() {
        const docId = $(this).attr("data-id");
        const nombreNorma = $(this).attr("data-nombre");
        const endpoint = window.API_BASE_URL + "/normas/open_doc";
        window.verDocumentoGlobal(endpoint, docId, `Norma: ${nombreNorma}`);
    });

    $("#btn-imprimir-informe").off("click").on("click", function() {
        window.print();
    });

    $("#btn-ver-vinculadas").off("click").on("click", async function() {
        const docId = $(this).attr("data-id");
        try {
            // Se asume que normas_vinculadas/list devuelve el json completo, filtramos en el cliente por id_norma_principal[cite: 29]
            const resp = await fetch(window.API_BASE_URL + "/normas_vinculadas/list");
            const lista = await resp.json();

            const vinculadas = lista.filter(item => item.id_norma_principal == docId);

            if (vinculadas.length === 0) {
                mostrarAlerta("No existen normas vinculadas a este registro.", "Normas Vinculadas");
                return;
            }

            let htmlVinculadas = `<ul class="list-group" style="margin-bottom:0;">`;
            vinculadas.forEach(v => {
                const nombreArchivoLimpio = v.path_name.split('/').pop();
                htmlVinculadas += `
                <li class="list-group-item" style="background-color: #2b2d35; border-color: #3f424e; display: flex; justify-content: space-between; align-items: center;">
                    <span style="color: #e0e0e0;">${nombreArchivoLimpio}</span>
                    <button class="btn btn-xs btn-info btn-abrir-vinculada" data-path="${v.path_name}">Ver</button>
                </li>`;
            });
            htmlVinculadas += `</ul>`;

            mostrarAlerta(htmlVinculadas, "Normas Vinculadas", true);

            // TODO: Cuando se cree el endpoint de open_doc en normas_vinculadas_service.c, se vincula aquí.
            $(".btn-abrir-vinculada").on("click", function() {
                mostrarAlerta("La función para abrir normas vinculadas está en desarrollo.", "Aviso");
            });

        } catch (error) {
            console.error(error);
            mostrarAlerta("Error al obtener las normas vinculadas.", "Error");
        }
    });
}

// ===================================================================================================================== //
// FUNCIÓN QUE CONSTRUYE EL FORMULARIO INTERNO Y MUESTRA EL MODAL FLOTANTE (ALTA/EDICIÓN)
// ===================================================================================================================== //
async function prepararYMostrarModal(modo, datos = null) {
    const $modal =$("#myModal-normas");
    if ($modal.length === 0) return;

    $modal.find(".modal-title").text(modo === "alta" ? "Añadir Norma" : "Editar Norma");

    $modal.find(".modal-body").html(`
        <style>
            .modern-dark-form {
                background-color: #1e1e24;
                padding: 20px 25px;
                border-radius: 8px;
                color: #e0e0e0;
                box-shadow: inset 0 0 10px rgba(0,0,0,0.5);
            }
            .modern-dark-form label {
                font-weight: 600;
                color: #9aa0a6;
                margin-bottom: 6px;
                font-size: 0.85em;
                text-transform: uppercase;
                letter-spacing: 0.5px;
            }
            .modern-dark-form .form-control {
                background-color: #2b2d35;
                border: 1px solid #3f424e;
                color: #ffffff;
                border-radius: 5px;
                transition: all 0.3s ease;
            }
            .modern-dark-form .form-control:focus {
                border-color: #5dade2;
                background-color: #343741;
                outline: 0;
                box-shadow: 0 0 0 0.15rem rgba(93, 173, 226, 0.25);
            }
            .modern-dark-form .form-control::placeholder { color: #6c757d; }
            .modern-dark-form .file-wrapper {
                background-color: #26282e;
                border: 1px dashed #5dade2;
                padding: 10px;
                border-radius: 5px;
                margin-top: 5px;
            }
        </style>

        <div class="modern-dark-form">
            <form id="form-modal-norma">
                <input type="hidden" id="modal-norma-id" value="${datos ? datos.id : ''}">

                <div class="row">
                    <div class="col-md-9">
                        <div class="form-group">
                            <label>Nombre Norma</label>
                            <input type="text" class="form-control" id="modal-norma-nombre_norma" value="${datos ? datos.nombre_norma : ''}" placeholder="Ej. Ley de Contrato de Trabajo" required>
                        </div>
                    </div>
                    <div class="col-md-3">
                        <div class="form-group">
                            <label>Nro. Norma</label>
                            <input type="text" class="form-control" id="modal-norma-n_norma" value="${datos ? datos.n_norma : ''}" required>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-md-6">
                        <div class="form-group">
                            <label>Tipo Norma</label>
                            <select class="form-control" id="modal-norma-tipo_norma" required></select>
                        </div>
                    </div>
                    <div class="col-md-6">
                        <div class="form-group">
                            <label>Ámbito Norma</label>
                            <select class="form-control" id="modal-norma-f_norma" required></select>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-md-6">
                        <div class="form-group">
                            <label>Fecha Publicación</label>
                            <input type="date" class="form-control" id="modal-norma-f_pub" value="${datos ? datos.f_pub : ''}" required>
                        </div>
                    </div>
                    <div class="col-md-6">
                        <div class="form-group">
                            <label>Año Publicación</label>
                            <input type="text" class="form-control" id="modal-norma-anio_pub" value="${datos ? datos.anio_pub : ''}" maxlength="4" placeholder="Ej. 1974" required>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-md-6">
                        <div class="form-group">
                            <label>Jurisdicción</label>
                            <select class="form-control" id="modal-norma-jurisdiccion" required></select>
                        </div>
                    </div>
                    <div class="col-md-6">
                        <div class="form-group">
                            <label>Organismo</label>
                            <select class="form-control" id="modal-norma-organismo" required></select>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-md-12">
                        <div class="form-group">
                            <label>Ubicación Física</label>
                            <input type="text" class="form-control" id="modal-norma-ubicacion_fisica" value="${datos ? datos.ubicacion_fisica : ''}" placeholder="Ej. Archivo Central, Estante 3" required>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-md-12">
                        <div class="form-group">
                            <label>Observaciones</label>
                            <textarea class="form-control" id="modal-norma-observaciones" rows="3" placeholder="Comentarios adicionales..." required>${datos ? datos.observaciones : ''}</textarea>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-md-12">
                        <div class="form-group file-wrapper">
                            <label>Documento Adjunto (PDF, DOC)</label>
                            <input type="file" class="form-control" style="border:none; padding-left:0;" id="modal-norma-file_name" accept=".pdf,.doc,.docx,.txt" ${datos ? '' : 'required'}>
                            ${datos && datos.file_name ? `<small class="text-info" style="display:block; margin-top:5px;">Archivo actual: <strong>${datos.file_name}</strong>. (Seleccione uno nuevo solo si desea reemplazarlo).</small>` : ''}
                        </div>
                    </div>
                </div>

                <button type="submit" id="btn-submit-oculto" style="display:none;"></button>
            </form>
        </div>
    `);

    $modal.find(".modal-footer").html(`
        <div id="modal-status-message" style="margin-bottom: 10px; text-align: left; font-weight: bold;"></div>
        <div class="text-right">
            <button type="button" class="btn btn-danger" data-dismiss="modal">
                <span class="glyphicon glyphicon-remove-circle" aria-hidden="true"></span> Cerrar</button>
            <button type="button" class="btn btn-success" id="btn-modal-guardar">
                <span class="glyphicon glyphicon-ok" aria-hidden="true"></span> Guardar</button>
        </div>
    `);

    // =====================================================================
    // 1. Extraemos los valores de forma segura
    // =====================================================================
    let valTipoNorma = "";
    let valAmbito = "";
    let valJurisdiccion = "";
    let valOrganismo = "";

    if (datos !== null) {
        valTipoNorma = datos.tipo_norma;
        valAmbito = datos.f_norma;
        valJurisdiccion = datos.jurisdiccion;
        valOrganismo = datos.organismo;
    }

    // 2. Cargamos los combos
    await cargarComboTipoNorma(valTipoNorma);
    await cargarComboAmbitoNorma(valAmbito);
    await cargarComboJurisdicciones(valJurisdiccion);
    await cargarComboOrganismos(valOrganismo);

    // Mostramos el modal
    $modal.modal("show");

    // Vinculamos el botón verde
    document.getElementById("btn-modal-guardar").onclick = () => {
        document.getElementById("btn-submit-oculto").click();
    };

    // =====================================================================
    // PROCESAMIENTO DEL FORMULARIO Y SUBIDA DE ARCHIVO BINARIO
    // =====================================================================
    document.getElementById("form-modal-norma").onsubmit = async (e) => {
        e.preventDefault();

        const statusContainer = document.getElementById("modal-status-message");
        statusContainer.innerHTML = `<span class="text-info">⏳ Procesando solicitud y subiendo archivo...</span>`;

        const id = document.getElementById("modal-norma-id").value;
        const fileInput = document.getElementById("modal-norma-file_name");

        const payload = {
            nombre_norma: document.getElementById("modal-norma-nombre_norma").value,
            n_norma: document.getElementById("modal-norma-n_norma").value,
            tipo_norma: document.getElementById("modal-norma-tipo_norma").value,
            f_norma: document.getElementById("modal-norma-f_norma").value,
            f_pub: document.getElementById("modal-norma-f_pub").value,
            anio_pub: document.getElementById("modal-norma-anio_pub").value,
            jurisdiccion: document.getElementById("modal-norma-jurisdiccion").value,
            organismo: document.getElementById("modal-norma-organismo").value,
            ubicacion_fisica: document.getElementById("modal-norma-ubicacion_fisica").value,
            observaciones: document.getElementById("modal-norma-observaciones").value
        };

        // Función interna que agrupa el envío para usarla con o sin archivo adjunto nuevo
        const executeFetch = async (finalFileName, finalFileData) => {
            try {
                const endpoint = id == "" ? "/normas/add" : "/normas/edit";

                let bodyData = `id=${encodeURIComponent(id)}&nombre_norma=${encodeURIComponent(payload.nombre_norma)}&n_norma=${encodeURIComponent(payload.n_norma)}&tipo_norma=${encodeURIComponent(payload.tipo_norma)}&f_norma=${encodeURIComponent(payload.f_norma)}&f_pub=${encodeURIComponent(payload.f_pub)}&anio_pub=${encodeURIComponent(payload.anio_pub)}&jurisdiccion=${encodeURIComponent(payload.jurisdiccion)}&organismo=${encodeURIComponent(payload.organismo)}&ubicacion_fisica=${encodeURIComponent(payload.ubicacion_fisica)}&observaciones=${encodeURIComponent(payload.observaciones)}&file_name=${encodeURIComponent(finalFileName)}`;

                // Solo adjuntamos los bytes si hay un archivo nuevo
                if (finalFileData !== "") {
                    bodyData += `&file_data=${encodeURIComponent(finalFileData)}`;
                }

                const enviorresp = await fetch(window.API_BASE_URL + endpoint, {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: bodyData
                });

                const data = await enviorresp.json();

                if (enviorresp.ok && data.status === "ok") {
                    statusContainer.innerHTML = `<span class="text-success"><strong>✅ ¡Éxito!</strong> ${data.message}.</span>`;
                    setTimeout(() => {
                        $modal.modal("hide");
                        window.loadDashboardView(window.VIEWS_PATH + "/normas/normas.html");
                    }, 1200);
                } else {
                    statusContainer.innerHTML = `<span class="text-danger"><strong>❌ Error:</strong> ${data.message}.</span>`;
                }
            } catch (err) {
                console.error(err);
                statusContainer.innerHTML = `<span class="text-danger"><strong>💥 Error:</strong> Fallo al conectar con el servidor.</span>`;
            }
        };

        // Verificamos si el usuario seleccionó un archivo nuevo
        if (fileInput.files.length > 0) {
            const file = fileInput.files[0];
            const reader = new FileReader();

            // Leemos el archivo y lo convertimos a Base64
            reader.onload = async function(event) {
                const fileBase64 = event.target.result;
                await executeFetch(file.name, fileBase64);
            };
            reader.readAsDataURL(file);
        } else {
            // Edición sin cambio de archivo
            const existingFileName = datos ? datos.file_name : "";
            await executeFetch(existingFileName, "");
        }
    };
}

// ======================================================================================================================= //
async function cargarComboJurisdicciones(valorSeleccionado) {
    const select = document.getElementById("modal-norma-jurisdiccion");
    if (!select) return;

    try {
        const resp = await fetch(window.API_BASE_URL +"/jurisdicciones/list");
        const lista = await resp.json();

        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
        lista.forEach(item => {
            const cod = item.cod_jur ? item.cod_jur.trim() : "";
            const desc = item.descripcion ? item.descripcion.trim() : "";
            const opt = document.createElement("option");
            opt.value = desc;
            opt.textContent = `${cod} - ${desc}`;
            if (desc == valorSeleccionado) opt.selected = true;
            select.appendChild(opt);
        });
    } catch (err) {
        console.error("Error al cargar Jurisdicciones:", err);
        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
    }
}

// ======================================================================================================================= //
async function cargarComboOrganismos(valorSeleccionado) {
    const select = document.getElementById("modal-norma-organismo");
    if (!select) return;

    try {
        const resp = await fetch(window.API_BASE_URL +"/organismos/list");
        const lista = await resp.json();

        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
        lista.forEach(item => {
            const cod = item.cod_org ? item.cod_org.trim() : "";
            const desc = item.descripcion ? item.descripcion.trim() : "";
            const opt = document.createElement("option");
            opt.value = cod;
            opt.textContent = `${cod} - ${desc}`;
            if (cod == valorSeleccionado) opt.selected = true;
            select.appendChild(opt);
        });
    } catch (err) {
        console.error("Error al cargar Organismos:", err);
        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
    }
}

// ======================================================================================================================= //
async function cargarComboTipoNorma(valorSeleccionado) {
    const select = document.getElementById("modal-norma-tipo_norma");
    if (!select) return;

    try {
        const resp = await fetch(window.API_BASE_URL +"/tipo_norma/list");
        const lista = await resp.json();

        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
        lista.forEach(item => {
            const desc = item.descripcion ? item.descripcion.trim() : "";
            const opt = document.createElement("option");
            opt.value = desc;
            opt.textContent = `${desc}`;
            if (desc == valorSeleccionado) opt.selected = true;
            select.appendChild(opt);
        });
    } catch (err) {
        console.error("Error al cargar Tipo Norma:", err);
        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
    }
}

// ======================================================================================================================= //
async function cargarComboAmbitoNorma(valorSeleccionado) {
    const select = document.getElementById("modal-norma-f_norma");
    if (!select) return;

    try {
        const resp = await fetch(window.API_BASE_URL +"/ambito_norma/list");
        const lista = await resp.json();

        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
        lista.forEach(item => {
            const desc = item.descripcion ? item.descripcion.trim() : "";
            const opt = document.createElement("option");
            opt.value = desc;
            opt.textContent = `${desc}`;
            if (desc == valorSeleccionado) opt.selected = true;
            select.appendChild(opt);
        });
    } catch (err) {
        console.error("Error al cargar Ambito Norma:", err);
        select.innerHTML = `<option value="">-- Seleccionar --</option>`;
    }
}
