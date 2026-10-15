console.log("✅ Librería Global [Open Docs] Iniciada.");

// ===================================================================================================================== //
// 1. AUTO-INYECCIÓN DEL MODAL (Se crea solo si no existe en el DOM)
// ===================================================================================================================== //
function _asegurarModalVisor() {
    if ($("#modal-visor-documentos").length === 0) {
        const modalHtml = `
        <div class="modal fade" id="modal-visor-documentos" tabindex="-1" role="dialog" style="z-index: 10500;">
          <div class="modal-dialog modal-lg" role="document" style="width: 90%; max-width: 1200px; height: 90vh;">
            <div class="modal-content" style="background-color: #1e1e24; border: 1px solid #3f424e; border-radius: 8px; height: 100%;">

              <div class="modal-header" style="border-bottom: 1px solid #3f424e; background-color: #2b2d35; border-radius: 8px 8px 0 0;">
                <button type="button" class="close" data-dismiss="modal" aria-label="Close" style="color: #fff; opacity: 0.8; text-shadow: none;">
                    <span aria-hidden="true">&times;</span>
                </button>
                <h4 class="modal-title" style="color: #5dade2; font-weight: bold;">
                    <span class="glyphicon glyphicon-file"></span> <span id="visor-doc-title">Visor de Documentos</span>
                </h4>
              </div>

              <div class="modal-body" style="height: calc(100% - 56px); padding: 0; background-color: #343741; position: relative;">
                <!-- Overlay de carga interno -->
                <div id="visor-doc-loader" style="position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); color: #5dade2; display: none; text-align: center;">
                    <span class="glyphicon glyphicon-refresh" style="font-size: 2em; animation: spin 1s infinite linear;"></span>
                    <p style="margin-top: 10px; font-weight: bold;">Cargando documento...</p>
                </div>
                <!-- Iframe nativo -->
                <iframe id="visor-doc-iframe" src="" style="width: 100%; height: 100%; border: none;"></iframe>
              </div>

            </div>
          </div>
        </div>
        <style>
            @keyframes spin { 100% { transform: rotate(360deg); } }
        </style>`;
        $("body").append(modalHtml);
    }
}

// ===================================================================================================================== //
// 2. FUNCIÓN DE VISUALIZACIÓN INTERNA (Maneja el Blob y la memoria)
// ===================================================================================================================== //
function _abrirBlobEnIframe(blobBase, tituloDoc) {
    const urlVirtual = URL.createObjectURL(blobBase);

    $("#visor-doc-loader").hide();
    $("#visor-doc-title").text(tituloDoc);
    $("#visor-doc-iframe").attr("src", urlVirtual);

    // Limpiamos la memoria de la PC del cliente cuando cierra el modal
    $("#modal-visor-documentos").on('hidden.bs.modal', function () {
        $("#visor-doc-iframe").attr("src", "");
        URL.revokeObjectURL(urlVirtual);
        $(this).off('hidden.bs.modal');
    });
}

// ===================================================================================================================== //
// 3. MÉTODO PÚBLICO GLOBAL (El que usarán todos los módulos de eGesDoc)
// ===================================================================================================================== //
window.verDocumentoGlobal = async function(endpointUrl, docId, tituloDoc) {
    _asegurarModalVisor();

    // Mostramos el modal en modo "Cargando" inmediatamente
    $("#visor-doc-title").text("Conectando con el servidor...");
    $("#visor-doc-iframe").attr("src", "");
    $("#visor-doc-loader").show();
    $("#modal-visor-documentos").modal("show");

    try {
        const response = await fetch(endpointUrl, {
            method: "POST",
            headers: { "Content-Type": "application/x-www-form-urlencoded" },
            body: `id=${encodeURIComponent(docId)}`
        });

        if (response.ok) {
            const blob = await response.blob();
            _abrirBlobEnIframe(blob, tituloDoc);
        } else {
            $("#modal-visor-documentos").modal("hide"); // Cerramos el visor si falla
            const errorData = await response.json();

            // Usamos mostrarAlerta si existe en el entorno, sino un alert nativo de fallback
            if (typeof mostrarAlerta === "function") {
                mostrarAlerta(errorData.error, "Documento no disponible");
            } else {
                alert(`Error: ${errorData.error}`);
            }
        }
    } catch (error) {
        $("#modal-visor-documentos").modal("hide");
        console.error("Error de red al intentar abrir el documento:", error);

        if (typeof mostrarAlerta === "function") {
            mostrarAlerta("Fallo en la comunicación con el servidor.", "Error de Red");
        } else {
            alert("Fallo en la comunicación con el servidor.");
        }
    }
};
